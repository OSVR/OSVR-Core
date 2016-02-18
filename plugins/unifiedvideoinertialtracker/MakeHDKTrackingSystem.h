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
#include <iostream>

#undef OSVR_UVBI_DISABLE_AUTOCALIB

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
            ".**....*........"    //  5
          , "....**...*......"    //  6
          , ".*...**........."    //  3
          , ".........*....**"    //  4
          , "..*.....**......"    //  1
          , "*......**......."    //  2
          , "....*.*..*......"    // 10
          , ".*.*.*.........."    //  8
          , ".........*.**..."    //  9
          , "**...........*.."    //  7
          , "....*.*......*.."    // 11
          , "*.......*.*....."    // 28
          , ".*........*.*..."    // 27
          , ".*.........*.*.."    // 25
          , "..*..*.*........"    // 15
          , "....*...*.*....."    // 16
          , "...*.*........*."    // 17
          , "...*.....*.*...."    // 18
          , "....*......*..*."    // 19
          , "....*..*....*..."    // 20
          , "..*...*........*"    // 21
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

        static const auto SCALE_FACTOR = 0.001; // mm to m

        template <typename Scalar> using cvMatx33 = cv::Matx<Scalar, 3, 3>;

        /// Rotation/basis-change part.
        template <typename Scalar> inline cvMatx33<Scalar> getTransform() {
            auto ret = cvMatx33<Scalar>::eye();
            // flip sign of x and z axes to make the HDK coordinate system match
            // our desired one.
            ret(0, 0) = -1;
            ret(2, 2) = -1;
            return ret;
        }

        /// Add the scaling part.
        template <typename Scalar>
        inline cvMatx33<Scalar> getTransformAndScale() {
            return getTransform<Scalar>() *
                   (cvMatx33<Scalar>::eye() * SCALE_FACTOR);
        }

        /// Transform points: we scale in addition to rotation/basis change
        template <typename Scalar>
        inline cv::Point3_<Scalar>
        transformFromHDKData(cv::Point3_<Scalar> input) {
            static const cvMatx33<Scalar> xformMatrix =
                getTransformAndScale<Scalar>();
            return xformMatrix * input;
        }

        /// Transform vectors: we only apply rotation/basis change
        template <typename Scalar>
        inline cv::Vec<Scalar, 3>
            transformFromHDKData(cv::Vec<Scalar, 3> input) {
            static const cvMatx33<Scalar> xformMatrix = getTransform<Scalar>();
            return xformMatrix * input;
        }
    } // namespace

    inline std::unique_ptr<TrackingSystem>
    makeHDKTrackingSystem(ConfigParams const &params) {
        std::unique_ptr<TrackingSystem> sys(new TrackingSystem(params));

        auto hmd = sys->createTrackedBody();
        if (!hmd) {
            throw std::runtime_error(
                "Could not create a tracked body for the HMD!");
        }

        auto numFrontBeacons = OsvrHdkLedLocations_SENSOR0.size();
        auto numRearBeacons = OsvrHdkLedLocations_SENSOR1.size();
        auto const useRear = params.includeRearPanel;
        auto numBeacons = numFrontBeacons + (useRear ? numRearBeacons : 0);

        /// Start setting up the data.
        TargetSetupData data;
        data.setBeaconCount(numBeacons);

        /// Fill in the patterns.
        data.patterns = OsvrHdkLedIdentifier_SENSOR0_PATTERNS;

        if (useRear) {
            /// Patterns for the second part.
            data.patterns.insert(end(data.patterns),
                                 begin(OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                                 end(OsvrHdkLedIdentifier_SENSOR1_PATTERNS));
        }

        /// Scale from millimeters to meters, and make the coordinate system
        /// what we want.
        auto locationsEnd = std::transform(
            begin(OsvrHdkLedLocations_SENSOR0),
            end(OsvrHdkLedLocations_SENSOR0), begin(data.locations),
            [](LocationPoint pt) { return transformFromHDKData(pt); });

        if (useRear) {
            // distance between front and back panel target origins, in mm,
            // because we'll apply this before converting coordinate systems.
            auto distanceBetweenPanels =
                static_cast<float>(params.headCircumference / M_PI * 10.f +
                                   params.headToFrontBeaconOriginDistance);

            /// Put on the back points too.
            auto rotate180aboutY = [](LocationPoint pt) {
                return LocationPoint(-pt.x, pt.y, -pt.z);
            };
            auto transformBackPoints = [distanceBetweenPanels,
                                        &rotate180aboutY](LocationPoint pt) {
                auto p = rotate180aboutY(pt) -
                         LocationPoint(0, 0, distanceBetweenPanels);
                return transformFromHDKData(p);
            };
            std::transform(begin(OsvrHdkLedLocations_SENSOR1),
                           end(OsvrHdkLedLocations_SENSOR1), locationsEnd,
                           transformBackPoints);
        }

        std::transform(
            begin(OsvrHdkLedDirections_SENSOR0),
            end(OsvrHdkLedDirections_SENSOR0), begin(data.emissionDirections),
            [](EmissionDirectionVec v) { return transformFromHDKData(v); });
        if (useRear) {
            // Resize down - so we can resize up to fill
            // those last elements with constant values.
            data.emissionDirections.resize(numFrontBeacons);
            data.emissionDirections.resize(
                numBeacons,
                transformFromHDKData(EmissionDirectionVec(0, 0, -1)));
        }
/// Set up autocalib.
/// Set the ones that are fixed.

#ifdef OSVR_UVBI_DISABLE_AUTOCALIB
        for (decltype(numBeacons) i = 0; i < numBeacons; ++i) {
            /// mark everybody fixed for testing.
            data.markBeaconFixed(makeOneBased(ZeroBasedBeaconId(i)));
        }
#else
        // for (auto idx : {16, 17, 34}) {
        for (auto idx : {17, 34}) {
            data.markBeaconFixed(OneBasedBeaconId(idx));
        }
#endif

        /// Put in the measurement variances.
        std::copy(begin(OsvrHdkLedVariances_SENSOR0),
                  end(OsvrHdkLedVariances_SENSOR0),
                  begin(data.baseMeasurementVariances));

        /// Clean, validate, and print a summary of the data.
        auto summary = data.cleanAndValidate();

        std::cout << summary << std::endl;

        auto opticalTarget = hmd->createTarget(
#if 0
            Eigen::Vector3d(0, 0, 0.04141),
#else
            Eigen::Vector3d::Zero(),
#endif
            data);

        if (!opticalTarget) {
            throw std::runtime_error(
                "Could not create a tracked target for the HMD!");
        }

        if (!params.imu.path.empty()) {
            auto imu =
                hmd->createIntegratedIMU(params.imu.orientationVariance,
                                         params.imu.angularVelocityVariance);
            if (!imu) {
                throw std::runtime_error(
                    "Could not create an integrated IMU object for the HMD!");
            }
        }
        return sys;
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
