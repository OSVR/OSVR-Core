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
#include "Types.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace vbtracker {

    /// Data for a single beacon, swizzled into a format suitable for "Vector of
    /// structs" usage. It is unswizzled/reswizzled as needed in setup.
    struct BeaconSetupData {
        /// The pattern of bright and dim, represented by * and . respectively,
        /// identifying this beacon.
        std::string pattern;
        /// The location of this beacon in the target coordinate system.
        cv::Point3f location;
        /// The direction that the beacon emits, in the target coordinate system
        cv::Vec3f emissionDirection;
        /// Initial measurement variance before applying observation-based
        /// modifiers.
        double baseMeasurementVariance = 3.0;
        /// Initial error in the beacon position state.
        double initialAutocalibrationError = 0.001;
        /// Is this beacon fixed, that is, not subject to autocalibration?
        bool isFixed = false;
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
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BeaconSetupData_h_GUID_AEDF8B01_FC4D_4388_2C88_0351E5E7FD83
