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
#include "BeaconSetupData.h"
#include "ConfigParams.h"
#include "HDKData.h"
#include "RangeTransform.h"
#include "TrackedBody.h"
#include "TrackingSystem.h"

#include <cvToEigen.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <memory>
#include <stdexcept>

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
        template <typename Scalar> inline cvMatx33<Scalar> get180AboutY() {
#if 0
            cvMatx33<Scalar> ret;
            map(ret) = Eigen::AngleAxis<Scalar>(
                           Scalar(M_PI), Eigen::Matrix<Scalar, 3, 1>::UnitY())
                           .toRotationMatrix();
#else
            auto ret = cvMatx33<Scalar>::eye();
            // flip sign of x and z axes to make the HDK coordinate system match
            // our desired one: this is equivalent to 180 degree rotation about
            // y.
            ret(0, 0) = -1;
            ret(2, 2) = -1;
#endif
            return ret;
        }

        /// Rotation/basis-change part.
        template <typename Scalar> inline cvMatx33<Scalar> getTransform() {
            return get180AboutY<Scalar>();
        }

        /// Add the scaling part.
        template <typename Scalar>
        inline cvMatx33<Scalar> getTransformAndScale() {
            return getTransform<Scalar>() * SCALE_FACTOR;
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

        inline LocationPoint rotatePoint180AboutY(LocationPoint pt) {
            pt.x *= -1;
            pt.z *= -1;
            return pt;
        }
    } // namespace

    inline std::unique_ptr<TrackingSystem>
    makeHDKTrackingSystem(ConfigParams const &params) {
        std::unique_ptr<TrackingSystem> sys(new TrackingSystem(params));
#ifdef OSVR_UVBI_DEBUG_EMISSION_DIRECTION
        {
            auto xform = getTransform<float>();
            std::cout << "Transform matrix:\n" << xform << std::endl;
        }
        /// Left, forward, top, right
        auto sampleBeacons = {5, 32, 9, 10};
#endif

        auto hmd = sys->createTrackedBody();
        if (!hmd) {
            throw std::runtime_error(
                "Could not create a tracked body for the HMD!");
        }

        const auto numFrontBeacons = OsvrHdkLedLocations_SENSOR0.size();
        const auto numRearBeacons = OsvrHdkLedLocations_SENSOR1.size();
        const auto useRear = params.includeRearPanel;
        const auto numBeacons =
            numFrontBeacons + (useRear ? numRearBeacons : 0);

        /// Start setting up the data.
        TargetSetupData data;
        data.setBeaconCount(numBeacons, BaseMeasurementVariance,
                            params.initialBeaconError);

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
        auto locationsEnd = range_transform(
            OsvrHdkLedLocations_SENSOR0, begin(data.locations),
            [](LocationPoint pt) { return transformFromHDKData(pt); });

        if (useRear) {
            // distance between front and back panel target origins, in mm,
            // because we'll apply this before converting coordinate systems.
            // Yes, all these transformations have been checked.
            const auto distanceBetweenPanels =
                static_cast<float>(params.headCircumference / M_PI * 10.f +
                                   params.headToFrontBeaconOriginDistance);

            /// Put on the back points too.
            auto transformBackPoints =
                [distanceBetweenPanels](LocationPoint pt) {
                    auto p = rotatePoint180AboutY(pt) -
                             LocationPoint(0, 0, distanceBetweenPanels);
                    return transformFromHDKData(p);
                };
            range_transform(OsvrHdkLedLocations_SENSOR1, locationsEnd,
                            transformBackPoints);
        }

        /// Put the emission directions on.
        range_transform(
            OsvrHdkLedDirections_SENSOR0, begin(data.emissionDirections),
            [](EmissionDirectionVec v) { return transformFromHDKData(v); });

        if (useRear) {
            // Resize down - so we can resize up to fill
            // those last elements with constant values.
            data.emissionDirections.resize(numFrontBeacons);
            data.emissionDirections.resize(
                numBeacons,
                transformFromHDKData(EmissionDirectionVec(0, 0, -1)));
        }
#ifdef OSVR_UVBI_DEBUG_EMISSION_DIRECTION
        for (auto &beaconOneBased : sampleBeacons) {
            std::cout << "Beacon ID " << beaconOneBased
                      << " emission direction "
                      << data.emissionDirections[beaconOneBased - 1]
                      << std::endl;
        }
#endif
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

        /// Clean and validate the data.
        auto summary = data.cleanAndValidate(params.silent);

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

        auto wantIMU =
            !params.imu.path.empty() &&
            (params.imu.useAngularVelocity || params.imu.useOrientation);
        if (wantIMU) {
            auto imu =
                hmd->createIntegratedIMU(params.imu.orientationVariance,
                                         params.imu.angularVelocityVariance);
            if (!imu) {
                throw std::runtime_error(
                    "Could not create an integrated IMU object for the HMD!");
            }
        } else {
#if 1
            sys->setCameraPose(Eigen::Isometry3d(Eigen::Translation3d(
                Eigen::Vector3d::Map(params.cameraPosition))));
#else
            sys->setCameraPose(Eigen::Isometry3d::Identity());
#endif
        }
        return sys;
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_MakeHDKTrackingSystem_h_GUID_B7C07A08_2BF8_45B2_4545_39979BA637E5
