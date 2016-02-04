/** @file
    @brief Header

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

#ifndef INCLUDED_BeaconSetupData_h_GUID_AEDF8B01_FC4D_4388_2C88_0351E5E7FD83
#define INCLUDED_BeaconSetupData_h_GUID_AEDF8B01_FC4D_4388_2C88_0351E5E7FD83

// Internal Includes
#include "BeaconIdTypes.h"
#include "Types.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace vbtracker {
    using EmissionDirectionVec = ::cv::Vec3d;
    using LocationPoint = ::cv::Point3f;
    /// Data for a single beacon, swizzled into a format suitable for
    /// "Vector of structs" usage. It is unswizzled/reswizzled as needed in
    /// setup.
    struct BeaconSetupData {
        /// The pattern of bright and dim, represented by * and . respectively,
        /// identifying this beacon.
        std::string pattern;
        /// The location of this beacon in the target coordinate system.
        LocationPoint location;
        /// The direction that the beacon emits, in the target coordinate system
        EmissionDirectionVec emissionDirection;
        /// Initial measurement variance before applying observation-based
        /// modifiers.
        double baseMeasurementVariance = 3.e-6;
        /// Initial error in the beacon position state.
        double initialAutocalibrationError = 1e-9;
        /// Is this beacon fixed, that is, not subject to autocalibration?
        bool isFixed = false;
    };

    struct TargetDataSummary {
        std::vector<OneBasedBeaconId> disabledByPattern;
        std::vector<OneBasedBeaconId> disabledByEmptyPattern;
        std::vector<std::pair<OneBasedBeaconId, std::string>> errors;
        std::vector<OneBasedBeaconId> validBeacons;
    };

    /// Data for a full target (all the beacons), unswizzled into a "struct of
    /// vectors". All should be the same size, since they are parallel.
    struct TargetSetupData {
        std::vector<std::string> patterns;
        Point3Vector locations;
        Vec3Vector emissionDirections;
        std::vector<double> baseMeasurementVariances;
        std::vector<double> initialAutocalibrationErrors;
        std::vector<bool> isFixed;

        using size_type = std::vector<double>::size_type;

        size_type numBeacons() const { return locations.size(); }

        static LocationPoint getBogusLocation() {
            return LocationPoint(-10000, -10000, -314159);
        }
        /// Resizes all arrays to the numBeacons.
        /// Only populates baseMeasurementVariances,
        /// initialAutocalibrationErrors, and isFixed
        /// with semi-reasonable default values (no beacons fixed)
        void setBeaconCount(std::size_t numBeacons,
                            double baseMeasurementVariance = 0.003,
                            double initialAutocalibrationError = 0.001) {
            patterns.resize(numBeacons);
            locations.resize(numBeacons, getBogusLocation());
            // these are invalid directions and must be populated!
            emissionDirections.resize(numBeacons,
                                      EmissionDirectionVec(0, 0, 0));
            baseMeasurementVariances.resize(numBeacons,
                                            baseMeasurementVariance);
            initialAutocalibrationErrors.resize(numBeacons,
                                                initialAutocalibrationError);
            isFixed.resize(numBeacons, false);
        }

        /// Mark a beacon, by zero-based ID, as being fixed.
        void markBeaconFixed(ZeroBasedBeaconId beacon) {
            isFixed.at(beacon.value()) = true;
            initialAutocalibrationErrors.at(beacon.value()) = 0;
        }
        /// Mark a beacon, by one-based ID, as being fixed.
        void markBeaconFixed(OneBasedBeaconId beacon) {
            markBeaconFixed(makeZeroBased(beacon));
        }

        TargetDataSummary cleanAndValidate();
    };

    /// Output operator for a target data summary.
    template <typename Stream>
    inline Stream &operator<<(Stream &os, TargetDataSummary const &summary) {
        os << "\n\nTarget Data Summary:\n";
        os << "\nBeacons disabled by their pattern:\n";
        for (auto id : summary.disabledByPattern) {
            os << id.value() << "\n";
        }
        os << "\nBeacons disabled by empty pattern:\n";
        for (auto id : summary.disabledByEmptyPattern) {
            os << id.value() << "\n";
        }
        os << "\nBeacons with errors:\n";
        for (auto &err : summary.errors) {
            os << err.first.value() << ": " << err.second << "\n";
        }
        os << "\nValid beacons:\n";
        for (auto id : summary.validBeacons) {
            os << id.value() << " ";
        }
        os << "\n\n";
        return os;
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BeaconSetupData_h_GUID_AEDF8B01_FC4D_4388_2C88_0351E5E7FD83
