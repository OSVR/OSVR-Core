/** @file
    @brief Implementation

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

// Internal Includes
#include "HDKData.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    // clang-format off
    // Default 3D locations for the beacons on an OSVR HDK face plate, in
    // millimeters
    // LED9, LED10, LED14, and LED15 had their Z spec originally at 13.09,
    // but the data here was changed during development with the comment
    // "new position works better"

    const Point3Vector OsvrHdkLedLocations_SENSOR0 = {
        cv::Point3f{-84.93f,   2.65f,  23.59f}, // LED7
        cv::Point3f{-83.27f, -14.33f,  13.89f}, // LED8
        cv::Point3f{-47.00f,  51.00f,  24.09f}, // LED9
        cv::Point3f{ 47.00f,  51.00f,  24.09f}, // LED10
        cv::Point3f{ 84.93f,   2.65f,  23.59f}, // LED11
        cv::Point3f{ 83.27f, -14.33f,  13.89f}, // LED12
        cv::Point3f{ 84.10f,  19.76f,  13.89f}, // LED13
        cv::Point3f{ 21.00f,  51.00f,  24.09f}, // LED14
        cv::Point3f{-21.00f,  51.00f,  24.09f}, // LED15
        cv::Point3f{-84.10f,  19.76f,  13.89f}, // LED16
        cv::Point3f{-60.41f,  47.55f,  44.60f}, // LED17
        cv::Point3f{-80.42f,  20.48f,  42.90f}, // LED18
        cv::Point3f{-82.01f,   2.74f,  42.40f}, // LED19
        cv::Point3f{-80.42f, -14.99f,  42.90f}, // LED20
        cv::Point3f{-60.41f, -10.25f,  48.10f}, // LED21
        cv::Point3f{-60.41f,  15.75f,  48.10f}, // LED22
        cv::Point3f{-30.41f,  32.75f,  50.50f}, // LED23
        cv::Point3f{-31.41f,  47.34f,  47.00f}, // LED24
        cv::Point3f{ -0.41f, -15.25f,  51.30f}, // LED25
        cv::Point3f{-30.41f, -27.25f,  50.50f}, // LED26
        cv::Point3f{-60.44f, -41.65f,  45.10f}, // LED27
        cv::Point3f{-22.41f, -41.65f,  47.80f}, // LED28
        cv::Point3f{ 21.59f, -41.65f,  47.80f}, // LED29
        cv::Point3f{ 59.59f, -41.65f,  45.10f}, // LED30
        cv::Point3f{ 79.63f, -14.98f,  42.90f}, // LED31
        cv::Point3f{ 29.59f, -27.25f,  50.50f}, // LED32
        cv::Point3f{ 81.19f,   2.74f,  42.40f}, // LED33
        cv::Point3f{ 79.61f,  20.48f,  42.90f}, // LED34
        cv::Point3f{ 59.59f,  47.55f,  44.60f}, // LED35
        cv::Point3f{ 30.59f,  47.55f,  47.00f}, // LED36
        cv::Point3f{ 29.59f,  32.75f,  50.50f}, // LED37
        cv::Point3f{ -0.41f,  20.75f,  51.30f}, // LED38
        cv::Point3f{ 59.59f,  15.75f,  48.10f}, // LED39
        cv::Point3f{ 59.59f, -10.25f,  48.10f}  // LED40
    };

    // Default 3D locations for the beacons on an OSVR HDK back plate, in
    // millimeters
    // the first and fourth LEDs do not actually exist in production
    const Point3Vector OsvrHdkLedLocations_SENSOR1 = {
        cv::Point3f{  1.00f,  23.80f,   0.00f}, // LED1
        cv::Point3f{ 11.00f,   5.80f,   0.00f}, // LED2
        cv::Point3f{  9.00f, -23.80f,   0.00f}, // LED3
        cv::Point3f{  0.00f,  -8.80f,   0.00f}, // LED4
        cv::Point3f{ -9.00f, -23.80f,   0.00f}, // LED5
        cv::Point3f{-12.00f,   5.80f,   0.00f}  // LED6
    };

    /// Locations on the HDK2 chassis - for LEDs that the HDK2 lacks, the HDK1 values
    /// were used as placeholders.
    const Point3Vector OsvrHdk2LedLocations_SENSOR0 = {
        cv::Point3f{-86.62f,   0.00f,  24.10f}, // LED7
        cv::Point3f{-85.52f, -17.23f,  13.90f}, // LED8
        cv::Point3f{-47.00f,  48.23f,  24.10f}, // LED9
        cv::Point3f{ 47.00f,  48.23f,  24.10f}, // LED10
        cv::Point3f{ 84.82f,   0.00f,  23.60f}, // LED11
        cv::Point3f{ 83.24f, -17.23f,  13.90f}, // LED12
        cv::Point3f{ 84.12f,  17.11f,  13.90f}, // LED13
        cv::Point3f{ 21.00f,  48.23f,  24.15f}, // LED14
        cv::Point3f{-21.00f,  48.23f,  24.15f}, // LED15
        cv::Point3f{-85.22f,  17.06f,  13.90f}, // LED16
        cv::Point3f{-60.04f,  44.91f,  43.74f}, // LED17
        cv::Point3f{-80.42f,  20.48f,  42.90f}, // LED18
        cv::Point3f{-82.01f,   2.74f,  42.40f}, // LED19
        cv::Point3f{-80.42f, -14.99f,  42.90f}, // LED20
        cv::Point3f{-60.19f, -14.35f,  47.80f}, // LED21
        cv::Point3f{-60.17f,  11.65f,  47.70f}, // LED22
        cv::Point3f{-30.09f,  28.65f,  50.04f}, // LED23
        cv::Point3f{-31.33f,  44.81f,  46.03f}, // LED24
        cv::Point3f{  0.00f, -19.35f,  50.81f}, // LED25
        cv::Point3f{-30.12f, -31.35f,  50.06f}, // LED26
        cv::Point3f{-60.08f, -47.00f,  44.32f}, // LED27
        cv::Point3f{-22.00f, -47.11f,  46.89f}, // LED28
        cv::Point3f{ 22.06f, -47.06f,  47.06f}, // LED29
        cv::Point3f{ 60.08f, -47.20f,  44.13f}, // LED30
        cv::Point3f{ 79.63f, -14.98f,  42.90f}, // LED31
        cv::Point3f{ 30.10f, -31.35f,  50.01f}, // LED32
        cv::Point3f{ 81.19f,   2.74f,  42.40f}, // LED33
        cv::Point3f{ 79.61f,  20.48f,  42.90f}, // LED34
        cv::Point3f{ 60.31f,  45.05f,  43.53f}, // LED35
        cv::Point3f{ 31.05f,  44.89f,  45.99f}, // LED36
        cv::Point3f{ 30.13f,  28.65f,  50.04f}, // LED37
        cv::Point3f{  0.00f,  16.65f,  50.81f}, // LED38
        cv::Point3f{ 60.19f,  11.65f,  47.72f}, // LED39
        cv::Point3f{ 60.20f, -14.35f,  47.71f}  // LED40
    };
    const Point3Vector OsvrHdk2LedLocations_SENSOR1 = {
        cv::Point3f{  1.00f,  23.80f,   0.00f}, // LED1
        cv::Point3f{ 11.00f,   5.80f,   0.00f}, // LED2
        cv::Point3f{  9.00f, -23.80f,   0.00f}, // LED3
        cv::Point3f{  0.00f,  -8.80f,   0.00f}, // LED4
        cv::Point3f{ -9.00f, -23.80f,   0.00f}, // LED5
        cv::Point3f{-12.00f,   5.80f,   0.00f}  // LED6
    };
    // clang-format on

    const Vec3Vector OsvrHdkLedDirections_SENSOR0 = {
        cv::Vec3d{-1, 0, 0},
        cv::Vec3d{-1, 0, 0},
        cv::Vec3d{0, 1, 0},
        cv::Vec3d{0, 1, 0},
        cv::Vec3d{1, 0, 0},
        cv::Vec3d{1, 0, 0},
        cv::Vec3d{1, 0, 0},
        cv::Vec3d{0, 1, 0},
        cv::Vec3d{0, 1, 0},
        cv::Vec3d{-1, 0, 0},
        cv::Vec3d{0, 0.7071067812, 0.7071067812},
        cv::Vec3d{-0.7071067812, 0, 0.7071067812},
        cv::Vec3d{-0.7071067812, 0, 0.7071067812},
        cv::Vec3d{-0.7071067812, 0, 0.7071067812},
        cv::Vec3d{-0.7071067812, 0, 0.7071067812},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0.7071067812, 0.7071067812},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, -0.7071067812, 0.7071067812},
        cv::Vec3d{0, -0.7071067812, 0.7071067812},
        cv::Vec3d{0, -0.7071067812, 0.7071067812},
        cv::Vec3d{0, -0.7071067812, 0.7071067812},
        cv::Vec3d{0.7071067812, 0, 0.7071067812},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0.7071067812, 0, 0.7071067812},
        cv::Vec3d{0.7071067812, 0, 0.7071067812},
        cv::Vec3d{0, 0.7071067812, 0.7071067812},
        cv::Vec3d{0, 0.7071067812, 0.7071067812},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1}};

    const Vec3Vector OsvrHdkLedDirections_SENSOR1 = {
        cv::Vec3d{0, 0, 1}, cv::Vec3d{0, 0, 1}, cv::Vec3d{0, 0, 1},
        cv::Vec3d{0, 0, 1}, cv::Vec3d{0, 0, 1}, cv::Vec3d{0, 0, 1}};

    /// generated by python script in this directory.
    const std::vector<double> OsvrHdkLedVariances_SENSOR0 = {
        5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 3.0, 8.0,
        8.0, 8.0, 8.0, 3.0, 3.0, 8.0, 3.0, 3.0, 8.0, 8.0, 8.0, 8.0,
        8.0, 3.0, 8.0, 8.0, 8.0, 8.0, 3.0, 3.0, 3.0, 3.0};

    static const std::initializer_list<std::size_t> HDK2_Missing_Beacons = {
        12, 13, 14, 25, 26, 28};
    std::initializer_list<std::size_t> getOneBasedIDsOfMissingBeaconsHDK2() {
        return HDK2_Missing_Beacons;
    }

} // namespace vbtracker
} // namespace osvr
