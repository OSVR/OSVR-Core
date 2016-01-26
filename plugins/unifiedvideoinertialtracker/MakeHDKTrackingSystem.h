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

#ifndef INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
#define INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5

// Internal Includes
#include "TrackingSystem.h"
#include "TrackedBody.h"
#include "BeaconSetupData.h"
#include "ConfigParams.h"
#include "HDKData.h"

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <stdexcept>

namespace osvr {
namespace vbtracker {
    namespace {
        // clang-format off
    /// @brief Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
    /// These are from the as-built measurements.
    /// First number in comments is overall LED ID, second is LED ID in the
    /// sensor it was in
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
            "X.**....*........"    //  5
          , "X....**...*......"    //  6
          , ".*...**........."    //  3
          , ".........*....**"    //  4
          , "..*.....**......"    //  1
          , "*......**......."    //  2
          , "....*.*..*......"    // 10
          , ".*.*.*.........."    //  8
          , ".........*.**..."    //  9
          , "X**...........*.."    //  7
          , "....*.*......*.."    // 11
          , "X*.......*.*....."    // 28
          , "X.*........*.*..."    // 27
          , "X.*.........*.*.."    // 25
          , "..*..*.*........"    // 15
          , "....*...*.*....."    // 16
          , "...*.*........*."    // 17
          , "...*.....*.*...."    // 18
          , "....*......*..*."    // 19
          , "....*..*....*..."    // 20
          , "X..*...*........*"    // 21
          , "........*..*..*."    // 22
          , ".......*...*...*"    // 23
          , "......*...*..*.."    // 24
          , ".......*....*..*"    // 14
          , "..*.....*..*...."    // 26
          , "*....*....*....."    // 13
          , "...*....*...*..."    // 12
          , "..*.....*...*..."    // 29
          , "...*......*...*."    // 30
          , "***...*........*"    // 31
          , "...****..*......"    // 32
          , "*.*..........***"    // 33
          , "**...........***"    // 34
    };

    /// @brief Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
    /// These are from the as-built measurements.
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
            "X............**.."    // 37 31 // never actually turns on in production
          , "......**.*......"    // 38 32
          , ".............***"    // 39 33
          , "X..........*....."    // 40 34 // never actually turns on in production
          , "...*.......**..."    // 33 27
          , "...**.....*....."    // 34 28
    };
        // clang-format on
    } // namespace
    inline std::unique_ptr<TrackingSystem>
    makeHDKTrackingSystem(ConfigParams const &params) {
        std::unique_ptr<TrackingSystem> sys(new TrackingSystem(params));

        auto hmd = sys->createTrackedBody();
        if (!hmd) {
            throw std::runtime_error(
                "Could not create a tracked body for the HMD!");
        }
        auto opticalTarget = hmd->createTarget(
            Eigen::Isometry3d(Eigen::Translation3d(0, 0, 0.04141)));

        if (!opticalTarget) {
            throw std::runtime_error(
                "Could not create a tracked target for the HMD!");
        }

        TargetSetupData data;
        /// Fill in the patterns.
        data.patterns = OsvrHdkLedIdentifier_SENSOR0_PATTERNS;
        /// Patterns for the second part.
        data.patterns.insert(end(data.patterns),
                             begin(OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                             end(OsvrHdkLedIdentifier_SENSOR1_PATTERNS));

        /// Scale from millimeters to meters, and make the coordinate system
        /// what we want.
        auto transformPoints = [](cv::Point3f pt) {
            auto p = pt * .001;
            return cv::Point3f(-p.x, p.y, -p.z);
        };

        data.locations.resize(OsvrHdkLedLocations_SENSOR0.size() +
                              OsvrHdkLedLocations_SENSOR1.size());
        auto lastOf0 = std::transform(begin(OsvrHdkLedLocations_SENSOR0),
                                      end(OsvrHdkLedLocations_SENSOR0),
                                      begin(data.locations), transformPoints);
        // distance between front and back panel target origins, in m.
        auto distanceBetweenPanels = (params.headCircumference / M_PI * 10. +
                                      params.headToFrontBeaconOriginDistance) *
                                     .001;

        /// Put on the back points too.
        auto transformBackPoints = [distanceBetweenPanels](cv::Point3f pt) {
            auto p = pt * .001;
            p.z += distanceBetweenPanels;
            return p;
        };
        std::transform(begin(OsvrHdkLedLocations_SENSOR1),
                       end(OsvrHdkLedLocations_SENSOR1), lastOf0,
                       transformBackPoints);

        /// Just changes the basis.
        auto transformVector = [](cv::Vec3f v) {
            return cv::Vec3f(-v[0], v[1], -v[2]);
        };

        data.emissionDirections.resize(OsvrHdkLedDirections_SENSOR0.size());
        std::transform(begin(OsvrHdkLedDirections_SENSOR0),
                       end(OsvrHdkLedDirections_SENSOR0),
                       begin(data.emissionDirections), transformVector);
        data.emissionDirections.resize(OsvrHdkLedDirections_SENSOR0.size() + 6,
                                       cv::Vec3d(0, 0, 1));

        /// Set up autocalib.
        data.isFixed.resize(OsvrHdkLedLocations_SENSOR0.size() +
                                OsvrHdkLedLocations_SENSOR1.size(),
                            false);
        data.initialAutocalibrationErrors.resize(
            OsvrHdkLedLocations_SENSOR0.size() +
                OsvrHdkLedLocations_SENSOR1.size(),
            0.001);
        /// Set the ones that are fixed.
        for (auto idx : {16, 17, 19, 20}) {
            data.isFixed[idx] = true;
            data.initialAutocalibrationErrors[idx] = 0;
        }

        data.baseMeasurementVariances.resize(
            OsvrHdkLedLocations_SENSOR0.size() +
                OsvrHdkLedLocations_SENSOR1.size(),
            3. / 100.);
        std::transform(begin(OsvrHdkLedVariances_SENSOR0),
                       end(OsvrHdkLedVariances_SENSOR0),
                       begin(data.baseMeasurementVariances),
                       [](double s) { return s / 100.; });
        return sys;
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
