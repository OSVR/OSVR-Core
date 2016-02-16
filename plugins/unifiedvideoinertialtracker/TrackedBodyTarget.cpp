/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "TrackedBodyTarget.h"
#include "TrackedBody.h"
#include "LED.h"
#include "cvToEigen.h"
#include "HDKLedIdentifier.h"
#include "PoseEstimatorTypes.h"
#include "PoseEstimator_RANSAC.h"
#include "PoseEstimator_SCAATKalman.h"
#include "PoseEstimator_RANSACKalman.h"
#include "BodyTargetInterface.h"

// Library/third-party includes
#include <boost/assert.hpp>
#include <util/Stride.h>

// Standard includes
#include <iostream>

/// Define this to use the RANSAC Kalman instead of the autocalibrating SCAAT
/// Kalman, primarily for troubleshooting purposes.
#undef OSVR_RANSACKALMAN

namespace osvr {
namespace vbtracker {
    enum class TargetTrackingState {
        RANSAC,
        EnteringKalman,
        Kalman,
        RANSACWhenBlobDetected
    };

    enum class TargetHealthState {
        OK,
        StopTrackingErrorBoundsExceeded,
        StopTrackingLostSight
    };
    static const auto MAX_FRAMES_WITHOUT_BEACONS = 150;
    static const double MAX_POSITIONAL_ERROR_VARIANCE = 15.;
    class TargetHealthEvaluator {
      public:
        TargetHealthState operator()(BodyState const &bodyState,
                                     LedPtrList const &leds,
                                     TargetTrackingState trackingState) {
            if (leds.empty()) {
                m_framesWithoutValidBeacons++;
            } else {
                m_framesWithoutValidBeacons = 0;
            }

            // Eigen::Vector3d positionalError =
            // bodyState.errorCovariance().diagonal().head<3>();
            if (trackingState != TargetTrackingState::RANSAC) {
                double maxPositionalError =
                    bodyState.errorCovariance().diagonal().head<3>().maxCoeff();
                if (maxPositionalError > MAX_POSITIONAL_ERROR_VARIANCE) {
                    return TargetHealthState::StopTrackingErrorBoundsExceeded;
                }
            }

            if (m_framesWithoutValidBeacons > MAX_FRAMES_WITHOUT_BEACONS) {
                return TargetHealthState::StopTrackingLostSight;
            }
            return TargetHealthState::OK;
        }

      private:
        std::size_t m_framesWithoutValidBeacons = 0;
    };
    struct TrackedBodyTarget::Impl {
        Impl(ConfigParams const &params, BodyTargetInterface const &bodyIface)
            : bodyInterface(bodyIface), kalmanEstimator(params) {}
        BodyTargetInterface bodyInterface;
        LedGroup leds;
        LedPtrList usableLeds;
        LedIdentifierPtr identifier;
        RANSACPoseEstimator ransacEstimator;
        SCAATKalmanPoseEstimator kalmanEstimator;
#ifdef OSVR_RANSACKALMAN
        RANSACKalmanPoseEstimator ransacKalmanEstimator;
#endif

        TargetHealthEvaluator healthEval;

        TargetTrackingState trackingState = TargetTrackingState::RANSAC;
        bool hasPrev = false;
        osvr::util::time::TimeValue lastEstimate;
    };

    inline BeaconStateVec createBeaconStateVec(ConfigParams const &params,
                                               TargetSetupData const &setupData,
                                               Eigen::Vector3d &beaconOffset) {
        {
            /// Compute or retrieve the beacon offset.
            if (params.offsetToCentroid) {
                Eigen::Vector3d beaconSum = Eigen::Vector3d::Zero();
                auto bNum = size_t{0};
                for (auto &beacon : setupData.locations) {
                    beaconSum += cvToVector(beacon).cast<double>();
                    bNum++;
                }
                beaconOffset = beaconSum / bNum;
                if (params.debug) {
                    std::cout << "[Tracker Target] Computed beacon centroid: "
                              << beaconOffset.transpose() << std::endl;
                }
            } else {
                beaconOffset = Eigen::Vector3d::Map(params.manualBeaconOffset);
            }
        }
        /// Create the vector we'll return, and then the beacon state
        /// objects.
        using size_type = TargetSetupData::size_type;
        const auto n = setupData.numBeacons();
        BeaconStateVec beacons;
        beacons.reserve(n);
        Eigen::Vector3d location;
        for (size_type i = 0; i < n; ++i) {
            location = cvToVector(setupData.locations[i]).cast<double>() -
                       beaconOffset;
            BeaconStatePtr beacon(new BeaconState(
                location, Eigen::Vector3d::Constant(
                              setupData.initialAutocalibrationErrors[i])
                              .asDiagonal()));
            beacons.emplace_back(std::move(beacon));
        }
        return beacons;
    }

    TrackedBodyTarget::TrackedBodyTarget(TrackedBody &body,
                                         BodyTargetInterface const &bodyIface,
                                         Eigen::Vector3d const &targetToBody,
                                         TargetSetupData const &setupData,
                                         TargetId id)
        : m_body(body), m_id(id), m_targetToBody(targetToBody),
          m_beaconMeasurementVariance(setupData.baseMeasurementVariances),
          m_beaconFixed(setupData.isFixed),
          m_beaconEmissionDirection(setupData.emissionDirections),
          m_impl(new Impl(getParams(), bodyIface)) {

        /// Create the beacon state objects and initialize the beacon offset.
        m_beacons =
            createBeaconStateVec(getParams(), setupData, m_beaconOffset);
        /// Create the beacon debug data
        m_beaconDebugData.resize(m_beacons.size());

        {
            /// Create the LED identifier
            std::unique_ptr<OsvrHdkLedIdentifier> identifier(
                new OsvrHdkLedIdentifier(setupData.patterns));
            m_impl->identifier = std::move(identifier);
        }
        m_verifyInvariants();
#if 0
        /// Dump the beacon locations to console
        dumpBeaconsToConsole();
#endif
    }

    TrackedBodyTarget::~TrackedBodyTarget() {
#if 0
    dumpBeaconsToConsole();
#endif
    }

    BodyTargetId TrackedBodyTarget::getQualifiedId() const {
        return BodyTargetId(getBody().getId(), getId());
    }

    Eigen::Vector3d
    TrackedBodyTarget::getBeaconAutocalibPosition(ZeroBasedBeaconId i) const {
        BOOST_ASSERT(!i.empty());
        BOOST_ASSERT_MSG(i.value() < getNumBeacons(),
                         "Beacon ID must be less than number of beacons.");
        BOOST_ASSERT_MSG(i.value() >= 0,
                         "Beacon ID must not be a sentinel value!");
        return m_beacons.at(i.value())->stateVector() + m_beaconOffset;
    }

    Eigen::Vector3d
    TrackedBodyTarget::getBeaconAutocalibVariance(ZeroBasedBeaconId i) const {
        BOOST_ASSERT(!i.empty());
        BOOST_ASSERT_MSG(i.value() < getNumBeacons(),
                         "Beacon ID must be less than number of beacons.");
        BOOST_ASSERT_MSG(i.value() >= 0,
                         "Beacon ID must not be a sentinel value!");
        return m_beacons.at(i.value())->errorCovariance().diagonal();
    }

    std::size_t TrackedBodyTarget::processLedMeasurements(
        LedMeasurementVec const &undistortedLeds) {
        // std::list<LedMeasurement> measurements{begin(undistortedLeds),
        // end(undistortedLeds)};
        LedMeasurementVec measurements{undistortedLeds};

        /// Clear the "usableLeds" that will be populated in a later step, if we
        /// get that far.
        usableLeds().clear();

        if (getParams().streamBeaconDebugInfo) {
            /// Only bother resetting if anyone is actually going to receive the
            /// data.
            for (auto &data : m_beaconDebugData) {
                data.reset();
            }
        }

        auto usedMeasurements = std::size_t{0};
        const auto blobMoveThreshold = getParams().blobMoveThreshold;
        const auto blobsKeepIdentity = getParams().blobsKeepIdentity;
        auto &myLeds = m_impl->leds;

        const auto numBeacons = getNumBeacons();

        /// In theory this shouldn't happen, but there are checks
        /// scattered all over the code. Now we can say that it doesn't
        /// happen because we won't let any bad values escape this
        /// routine.
        auto handleOutOfRangeIds = [numBeacons](Led &led) {
            if (led.identified() &&
                makeZeroBased(led.getID()).value() > numBeacons) {
                std::cerr << "Got a beacon claiming to be "
                          << led.getOneBasedID().value()
                          << " when we only have " << numBeacons << " beacons"
                          << std::endl;
                /// @todo a kinder way of doing this? Right now this blows away
                /// the measurement history
                led.markMisidentified();
                return true;
            }
            return false;
        };

        auto led = begin(myLeds);
        while (led != end(myLeds)) {
            led->resetUsed();
            handleOutOfRangeIds(*led);
            auto threshold = blobMoveThreshold * led->getMeasurement().diameter;
            auto nearest = led->nearest(measurements, threshold);
            if (nearest == end(measurements)) {
                // We have no blob corresponding to this LED, so we need
                // to delete this LED.
                led = myLeds.erase(led);
            } else {
                // Update the values in this LED and then go on to the
                // next one. Remove this blob from the list of
                // potential matches.
                led->addMeasurement(*nearest, blobsKeepIdentity);
                if (!handleOutOfRangeIds(*led)) {
                    /// If that measurement didn't cause this beacon to go awry,
                    /// then we'll actually handle the measurement and increment
                    /// used measurements.
                    measurements.erase(nearest);
                    /// @todo do we increment this only if the LED is
                    /// recognized?
                    usedMeasurements++;
                }
                ++led;
            }
        }

        // If we have any blobs that have not been associated with an
        // LED, then we add a new LED for each of them.
        // std::cout << "Had " << Leds.size() << " LEDs, " <<
        // keyPoints.size() << " new ones available" << std::endl;
        for (auto &remainingLed : measurements) {
            myLeds.emplace_back(m_impl->identifier.get(), remainingLed);
        }
        return usedMeasurements;
    }

    bool TrackedBodyTarget::updatePoseEstimateFromLeds(
        CameraParameters const &camParams,
        osvr::util::time::TimeValue const &tv, BodyState &bodyState,
        osvr::util::time::TimeValue const &startingTime,
        bool validStateAndTime) {

        /// Do the initial filtering of the LED group to just the identified
        /// ones before we pass it to an estimator.
        updateUsableLeds();

        /// Must pre/post correct the state by our offset :-/
        /// @todo make this state correction less hacky.
        bodyState.position() += getStateCorrection();

        /// @todo put this in the class
        bool permitKalman = true && validStateAndTime;

        /// OK, now must decide who we talk to for pose estimation.
        /// @todo move state machine logic elsewhere

        if (!m_hasPoseEstimate &&
            m_impl->trackingState != TargetTrackingState::RANSAC) {
            /// Lost tracking.
            enterRANSACMode();
        }

        /// pre-estimation transitions based on overall health
        switch (m_impl->healthEval(bodyState, usableLeds(),
                                   m_impl->trackingState)) {
        case TargetHealthState::StopTrackingErrorBoundsExceeded:
            msg() << "In flight reset - error bounds exceeded..." << std::endl;
            enterRANSACMode();
            break;
        case TargetHealthState::StopTrackingLostSight:
#if 0
            msg() << "Lost sight of beacons for too long, awaiting their "
                     "return..."
                  << std::endl;
#endif
            enterRANSACMode();
            break;
        case TargetHealthState::OK:
            // we're ok, no transition needed.
            break;
        }
        /// Pre-estimation transitions per-state
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSACWhenBlobDetected: {
            if (!usableLeds().empty()) {
                msg()
                    << "In flight reset - beacons detected, re-acquiring fix..."
                    << std::endl;
                enterRANSACMode();
            }
            break;
        }
        default:
            // other states don't have pre-estimation transitions.
            break;
        }

        /// main estimation dispatch
        auto params = EstimatorInOutParams{camParams,
                                           m_beacons,
                                           m_beaconMeasurementVariance,
                                           m_beaconFixed,
                                           m_beaconEmissionDirection,
                                           startingTime,
                                           bodyState,
                                           getBody().getProcessModel(),
                                           m_beaconDebugData,
                                           m_targetToBody};
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSAC: {
            m_hasPoseEstimate = m_impl->ransacEstimator(params, usableLeds());
            break;
        }

        case TargetTrackingState::RANSACWhenBlobDetected:
        case TargetTrackingState::EnteringKalman:
        case TargetTrackingState::Kalman: {
#ifdef OSVR_RANSACKALMAN
            m_hasPoseEstimate =
                m_impl->ransacKalmanEstimator(params, usableLeds(), tv);
#else
            auto videoDt =
                osvrTimeValueDurationSeconds(&tv, &m_impl->lastEstimate);
            m_hasPoseEstimate =
                m_impl->kalmanEstimator(params, usableLeds(), tv, videoDt);
#endif
            break;
        }
        }

        /// post-estimation transitions (based on state)
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSAC: {
            if (m_hasPoseEstimate && permitKalman) {
                enterKalmanMode();
            }
            break;
        }
        case TargetTrackingState::EnteringKalman:
            m_impl->trackingState = TargetTrackingState::Kalman;
            // Get one frame pass on the Kalman health check.
            break;
        case TargetTrackingState::Kalman: {
#ifndef OSVR_RANSACKALMAN
            auto health = m_impl->kalmanEstimator.getTrackingHealth();
            switch (health) {
            case SCAATKalmanPoseEstimator::TrackingHealth::NeedsResetNow:
                msg() << "In flight reset - lost fix..." << std::endl;
                enterRANSACMode();
                break;
            case SCAATKalmanPoseEstimator::TrackingHealth::ResetWhenBeaconsSeen:
                m_impl->trackingState =
                    TargetTrackingState::RANSACWhenBlobDetected;
                break;
            case SCAATKalmanPoseEstimator::TrackingHealth::Functioning:
                // OK!
                break;
            }
#endif
            break;
        }
        default:
            // other states don't have post-estimation transitions.
            break;
        }

        /// Update our local target-specific timestamp
        m_impl->lastEstimate = tv;

        /// Corresponding post-correction.
        bodyState.position() -= getStateCorrection();

        return m_hasPoseEstimate;
    }

    bool TrackedBodyTarget::uncalibratedRANSACPoseEstimateFromLeds(
        CameraParameters const &camParams, Eigen::Vector3d &xlate,
        Eigen::Quaterniond &quat) {

        /// Do the initial filtering of the LED group to just the identified
        /// ones before we pass it to an estimator.
        updateUsableLeds();
        Eigen::Vector3d outXlate;
        Eigen::Quaterniond outQuat;
        auto gotPose =
            m_impl->ransacEstimator(camParams, usableLeds(), m_beacons,
                                    m_beaconDebugData, outXlate, outQuat);
        if (gotPose) {
            // Post-correct the state
            xlate = outXlate - outQuat * m_beaconOffset;
            // copy the quat
            quat = outQuat;
        }
        return gotPose;
    }

    Eigen::Vector3d TrackedBodyTarget::getStateCorrection() const {
        return m_impl->bodyInterface.state.getQuaternion() * m_beaconOffset;
    }

    ConfigParams const &TrackedBodyTarget::getParams() const {
        return m_body.getParams();
    }
    std::ostream &TrackedBodyTarget::msg() const {
        return std::cout << "[Tracker Target " << getQualifiedId() << "] ";
    }
    void TrackedBodyTarget::enterKalmanMode() {
        msg() << "Entering SCAAT Kalman mode..." << std::endl;
        m_impl->trackingState = TargetTrackingState::EnteringKalman;
        m_impl->kalmanEstimator.resetCounters();
    }

    void TrackedBodyTarget::enterRANSACMode() {

#ifndef OSVR_UVBI_ASSUME_SINGLE_TARGET_PER_BODY
#error                                                                         \
    "We may not be able/willing to run right over the body velocity just because this target lost its fix"
#endif
        // Zero out velocities if we're coming from Kalman.
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSACWhenBlobDetected:
        case TargetTrackingState::Kalman:
            getBody().getState().angularVelocity() = Eigen::Vector3d::Zero();
            getBody().getState().velocity() = Eigen::Vector3d::Zero();
            break;
        case TargetTrackingState::EnteringKalman:
            /// unlikely to have messed up velocity in one step. let it be.
            break;
        default:
            break;
        }
        m_impl->trackingState = TargetTrackingState::RANSAC;
    }

    LedGroup const &TrackedBodyTarget::leds() const { return m_impl->leds; }

    LedPtrList const &TrackedBodyTarget::usableLeds() const {
        return m_impl->usableLeds;
    }

    LedGroup &TrackedBodyTarget::leds() { return m_impl->leds; }

    LedPtrList &TrackedBodyTarget::usableLeds() { return m_impl->usableLeds; }
    void TrackedBodyTarget::updateUsableLeds() {
        auto &usable = usableLeds();
        usable.clear();
        auto &leds = m_impl->leds;
        for (auto &led : leds) {
            if (!led.identified()) {
                continue;
            }
            usable.push_back(&led);
        }
    }
    osvr::util::time::TimeValue const &
    TrackedBodyTarget::getLastUpdate() const {
        return m_impl->lastEstimate;
    }

    void TrackedBodyTarget::dumpBeaconsToConsole() const {

        /// Dump the beacon locations to console in a CSV-like format.
        auto numBeacons = getNumBeacons();
        std::cout << "BeaconsID,x,y,z" << std::endl;
        Eigen::IOFormat ourFormat(Eigen::StreamPrecision, 0, ",");
        for (UnderlyingBeaconIdType i = 0; i < numBeacons; ++i) {
            auto id = ZeroBasedBeaconId(i);
            std::cout << i + 1 << ","
                      << getBeaconAutocalibPosition(id).transpose().format(
                             ourFormat)
                      << "\n";
        }
    }

} // namespace vbtracker
} // namespace osvr
