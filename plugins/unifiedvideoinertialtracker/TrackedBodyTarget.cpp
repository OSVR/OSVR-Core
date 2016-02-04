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
#include "BodyTargetInterface.h"

// Library/third-party includes
#include <boost/assert.hpp>
#include <util/Stride.h>

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    enum class TargetTrackingState { RANSAC, Kalman, RANSACWhenBlobDetected };
    struct TrackedBodyTarget::Impl {
        Impl(ConfigParams const &params, BodyTargetInterface const &bodyIface)
            : bodyInterface(bodyIface), kalmanEstimator(params) {}
        BodyTargetInterface bodyInterface;
        LedGroup leds;
        LedPtrList usableLeds;
        LedIdentifierPtr identifier;
        RANSACPoseEstimator ransacEstimator;
        SCAATKalmanPoseEstimator kalmanEstimator;

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
                                         Eigen::Isometry3d const &targetToBody,
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

#if 0
        /// Dump the beacon locations to console
        dumpBeaconsToConsole();
#endif
    }

    TrackedBodyTarget::~TrackedBodyTarget() { dumpBeaconsToConsole(); }

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
        m_impl->usableLeds.clear();

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
        osvr::util::time::TimeValue const &tv) {

        /// Do the initial filtering of the LED group to just the identified
        /// ones before we pass it to an estimator.
        auto &usable = usableLeds();
        auto &leds = m_impl->leds;
        for (auto &led : leds) {
            if (!led.identified()) {
                continue;
            }
            usable.push_back(&led);
        }
        /// Must pre/post correct the state by our offset :-/
        /// @todo make this state correction less hacky.
        m_impl->bodyInterface.state.position() += getStateCorrection();

        /// @todo put this in the class
        bool permitKalman = true;

        /// OK, now must decide who we talk to for pose estimation.
        /// @todo move state machine logic elsewhere

        // pre-estimation transitions
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSACWhenBlobDetected: {
            if (!usable.empty()) {
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

        // main estimation dispatch

        auto params = EstimatorInOutParams{m_beacons,
                                           m_beaconMeasurementVariance,
                                           m_beaconFixed,
                                           m_beaconEmissionDirection,
                                           getBody().getState(),
                                           getBody().getProcessModel(),
                                           m_beaconDebugData};
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSAC: {
            m_hasPoseEstimate =
                m_impl->ransacEstimator(camParams, usable, params);
            break;
        }

        case TargetTrackingState::RANSACWhenBlobDetected:
        case TargetTrackingState::Kalman: {
            auto videoDt =
                osvrTimeValueDurationSeconds(&tv, &m_impl->lastEstimate);
            m_hasPoseEstimate =
                m_impl->kalmanEstimator(camParams, usable, videoDt, params);
            break;
        }
        }

        // post-estimation transitions
        switch (m_impl->trackingState) {
        case TargetTrackingState::RANSAC: {
            if (m_hasPoseEstimate && permitKalman) {
                enterKalmanMode();
            }
            break;
        }
        case TargetTrackingState::Kalman: {
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
        }
        default:
            // other states don't have post-estimation transitions.
            break;
        }

        /// Update our local target-specific timestamp
        m_impl->lastEstimate = tv;

        /// Corresponding post-correction.
        m_impl->bodyInterface.state.position() -= getStateCorrection();

        return m_hasPoseEstimate;
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
        m_impl->trackingState = TargetTrackingState::Kalman;
        m_impl->kalmanEstimator.resetCounters();
    }

    void TrackedBodyTarget::enterRANSACMode() {
        m_impl->trackingState = TargetTrackingState::RANSAC;
    }

    LedGroup const &TrackedBodyTarget::leds() const { return m_impl->leds; }

    LedPtrList const &TrackedBodyTarget::usableLeds() const {
        return m_impl->usableLeds;
    }

    LedGroup &TrackedBodyTarget::leds() { return m_impl->leds; }

    LedPtrList &TrackedBodyTarget::usableLeds() { return m_impl->usableLeds; }

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
