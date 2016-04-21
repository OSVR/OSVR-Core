/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_HDKData_h_GUID_89F92BAC_AB45_44B8_1402_D4F4D5E3A9B9
#define INCLUDED_HDKData_h_GUID_89F92BAC_AB45_44B8_1402_D4F4D5E3A9B9

// Internal Includes
#include "Types.h"

// Library/third-party includes
// - none

// Standard includes
#define _USE_MATH_DEFINES
#include <math.h>

namespace osvr {
namespace vbtracker {
    /// @name Default 3D locations for the beacons on an OSVR HDK, in
    /// millimeters
    /// @{
    extern const Point3Vector OsvrHdkLedLocations_SENSOR0;
    extern const Point3Vector OsvrHdkLedLocations_SENSOR1;
    /// @}

    /// @name Emission directions for the beacons on an OSVR HDK
    /// @{
    extern const Vec3Vector OsvrHdkLedDirections_SENSOR0;
    extern const Vec3Vector OsvrHdkLedDirections_SENSOR1;
    /// @}

    extern const std::vector<double> OsvrHdkLedVariances_SENSOR0;

    inline std::size_t getNumHDKFrontPanelBeacons() {
        return OsvrHdkLedLocations_SENSOR0.size();
    }

    inline std::size_t getNumHDKRearPanelBeacons() {
        return OsvrHdkLedLocations_SENSOR1.size();
    }

    /// distance between front and back panel target origins, in mm.
    inline double
    computeDistanceBetweenPanels(double headCircumference,
                                 double headToFrontBeaconOriginDistance) {
        return headCircumference / M_PI * 10. + headToFrontBeaconOriginDistance;
    }

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_HDKData_h_GUID_89F92BAC_AB45_44B8_1402_D4F4D5E3A9B9
