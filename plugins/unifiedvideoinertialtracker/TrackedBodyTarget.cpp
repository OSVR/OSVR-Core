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
#include "BodyTargetInterface.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    struct TrackedBodyTarget::Impl {
        Impl(ConfigParams const &params, BodyTargetInterface const &bodyIface)
            : bodyInterface(bodyIface) {}
        BodyTargetInterface bodyInterface;
        LedGroup leds;
        LedPtrList usableLeds;
        LedIdentifierPtr identifier;
        RANSACPoseEstimator ransacEstimator;
    };

    namespace detail {
        inline BeaconStateVec
        createBeaconStateVec(ConfigParams const &params,
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
#if 0
                if (params.debug) {
                    std::cout << "Beacon centroid: " << m_centroid.transpose()
                        << std::endl;
                }
#endif
                } else {
                    beaconOffset =
                        Eigen::Vector3d::Map(params.manualBeaconOffset);
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
    } // namespace detail

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
        m_beacons = detail::createBeaconStateVec(getParams(), setupData,
                                                 m_beaconOffset);
        {
            /// Create the LED identifier
            std::unique_ptr<OsvrHdkLedIdentifier> identifier(
                new OsvrHdkLedIdentifier(setupData.patterns));
            m_impl->identifier = std::move(identifier);
        }
    }

    TrackedBodyTarget::~TrackedBodyTarget() {}

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

        auto usedMeasurements = std::size_t{0};
        const auto blobMoveThreshold = getParams().blobMoveThreshold;
        const auto blobsKeepIdentity = getParams().blobsKeepIdentity;
        auto &myLeds = m_impl->leds;

        auto led = begin(myLeds);
        while (led != end(myLeds)) {
            led->resetUsed();

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
                measurements.erase(nearest);
                ++led;

                /// @todo do we increment this only if the LED is recognized?
                usedMeasurements++;
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
        CameraParameters const &camParams) {

        /// Do the initial filtering of the LED group to just the identified,
        /// in-bounds-ID ones before we pass it to an estimator.
        auto &usable = usableLeds();
        auto const beaconsSize = static_cast<int>(m_beacons.size());
        auto &leds = m_impl->leds;
        for (auto &led : leds) {
            if (!led.identified()) {
                continue;
            }
            if (led.getID().value() > beaconsSize) {
                // out of bounds ID
                continue;
            }
            usable.push_back(&led);
        }

        /// Must pre/post correct the state by our offset :-/
        /// @todo make this state correction less hacky.
        m_impl->bodyInterface.state.position() += getStateCorrection();

        /// OK, now must decide who we talk to for pose estimation.
        /// @todo right now just ransac.
        m_hasPoseEstimate = m_impl->ransacEstimator(camParams, usable, m_beacons,
                                               m_impl->bodyInterface.state);

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

    LedGroup const &TrackedBodyTarget::leds() const { return m_impl->leds; }

    LedPtrList const &TrackedBodyTarget::usableLeds() const {
        return m_impl->usableLeds;
    }

    LedGroup &TrackedBodyTarget::leds() { return m_impl->leds; }

    LedPtrList &TrackedBodyTarget::usableLeds() { return m_impl->usableLeds; }

} // namespace vbtracker
} // namespace osvr
