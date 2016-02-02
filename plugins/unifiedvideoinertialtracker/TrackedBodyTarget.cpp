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

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    struct TrackedBodyTarget::Impl {
        LedGroup leds;
        LedIdentifierPtr identifier;
    };

    namespace detail {
        using BeaconState = kalman::PureVectorState<3>;
        using BeaconStatePtr = std::unique_ptr<BeaconState>;
        using BeaconStateVec = std::vector<BeaconStatePtr>;
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
                                         Eigen::Isometry3d const &targetToBody,
                                         TargetSetupData const &setupData)
        : m_body(body), m_targetToBody(targetToBody),
          m_beaconMeasurementVariance(setupData.baseMeasurementVariances),
          m_beaconFixed(setupData.isFixed),
          m_beaconEmissionDirection(setupData.emissionDirections),
          m_impl(new Impl) {

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

    Eigen::Vector3d
    TrackedBodyTarget::getBeaconAutocalibPosition(ZeroBasedBeaconId i) const {
        BOOST_ASSERT(!i.empty());
        BOOST_ASSERT_MSG(i.value() < getNumBeacons(),
                         "Beacon ID must be less than number of beacons.");
        BOOST_ASSERT_MSG(i.value() >= 0,
                         "Beacon ID must not be a sentinel value!");
        return m_beacons.at(i.value())->stateVector();
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
        std::vector<LedMeasurement> const &undistortedLeds) {
        // std::list<LedMeasurement> measurements{begin(undistortedLeds),
        // end(undistortedLeds)};
        std::vector<LedMeasurement> measurements{undistortedLeds};

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

    ConfigParams const &TrackedBodyTarget::getParams() const {
        return m_body.getParams();
    }

} // namespace vbtracker
} // namespace osvr
