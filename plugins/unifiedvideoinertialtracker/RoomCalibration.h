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

#ifndef INCLUDED_RoomCalibration_h_GUID_D53BD552_47D4_4390_C6AC_C6819180D4AF
#define INCLUDED_RoomCalibration_h_GUID_D53BD552_47D4_4390_C6AC_C6819180D4AF

// Internal Includes
#include "BodyIdTypes.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenFilters.h>
#include <osvr/Util/TimeValue.h>

#include <boost/optional.hpp>

// Standard includes
#include <cstddef>
#include <iosfwd>

#undef OSVR_USE_SECOND_EURO_FILTER

namespace osvr {
namespace vbtracker {
    class TrackingSystem;
    /// Takes care of the initial room calibration startup step, when we learn
    /// the pose of the camera in space and the yaw offset of the IMU.
    class RoomCalibration {
      public:
        RoomCalibration(Eigen::Vector3d const &camPosition,
                        bool cameraIsForward = true);

        /// Since during startup, we only want video data on a single target, we
        /// can save processing power by asking before we compute.
        bool wantVideoData(TrackingSystem const &sys,
                           BodyTargetId const &target) const;

        void processVideoData(TrackingSystem const &sys,
                              BodyTargetId const &target,
                              util::time::TimeValue const &timestamp,
                              Eigen::Vector3d const &xlate,
                              Eigen::Quaterniond const &quat);

        void processIMUData(TrackingSystem const &sys, BodyId const &body,
                            util::time::TimeValue const &timestamp,
                            Eigen::Quaterniond const &quat);

        /// When completed feeding data, this method will check to see if
        /// calibration has finished and perform updates accordingly.
        bool postCalibrationUpdate(TrackingSystem &sys);

        bool calibrationComplete() const { return m_calibComplete; }

        /// @name Accessors only valid once postCalibrationUpdate() has returned
        /// true!
        /// @{
        /// Gets the calibration yaw for the IMU on a body. Returns an empty
        /// optional if the body ID given was not the one with the IMU used to
        /// calibrate.
        boost::optional<util::Angle>
        getCalibrationYaw(BodyId const &body) const;
        /// Gets the pose of the camera in the room (the transform from camera
        /// space to room space)
        Eigen::Isometry3d getCameraPose() const;
        /// @}

      private:
        bool finished() const;

#ifdef OSVR_USE_SECOND_EURO_FILTER
        /// This gets a live transform from camera space to IMU space.
        Eigen::Isometry3d getCameraToIMUCalibrationPoint() const;
#endif

        /// The stream used by msg() and friends
        std::ostream &msgStream() const;
        /// A nicely prefixed stream
        std::ostream &msg() const;
        /// Use this (followed by a call to endInstructions) when you want to
        /// print out a long string of instructions set out from the surrounding
        /// text.
        std::ostream &instructions() const;
        void endInstructions() const;

        bool haveVideoData() const { return !m_videoTarget.first.empty(); }
        bool haveIMUData() const { return !m_imuBody.empty(); }
        std::size_t m_steadyVideoReports = 0;

        double m_linVel = 0;
        double m_angVel = 0;
        void handleExcessVelocity(double zTranslation);
        enum class InstructionState {
            Uninstructed,
            ToldToMoveCloser,
            ToldDistanceIsGood
        };
        InstructionState m_instructionState = InstructionState::Uninstructed;

        /// @name Video-based tracking data and input filters
        /// @{
        BodyTargetId m_videoTarget;
        util::time::TimeValue m_lastVideoData;
        /// @}

#ifdef OSVR_USE_SECOND_EURO_FILTER
        /// Filter on pose in camera space
        util::filters::PoseOneEuroFilterd m_poseFilter =
            util::filters::PoseOneEuroFilterd{
                util::filters::one_euro::Params{1, 0.05},
                util::filters::one_euro::Params{1}};

        /// Input filter on camera in room/IMU space
        util::filters::PoseOneEuroFilterd m_cameraFilter =
            util::filters::PoseOneEuroFilterd{
                util::filters::one_euro::Params{3, 0.1},
                util::filters::one_euro::Params{3}};
#else
        /// Filter on pose in camera space
        util::filters::PoseOneEuroFilterd m_poseFilter =
            util::filters::PoseOneEuroFilterd{
                util::filters::one_euro::Params{3, 0.03},
                util::filters::one_euro::Params{1, 0.01}};
        Eigen::Vector3d m_rTc_ln_accum = Eigen::Vector3d::Zero();
#endif
        BodyId m_imuBody;
        Eigen::Quaterniond m_imuOrientation = Eigen::Quaterniond::Identity();

        /// @name Supplied config
        /// @{
        Eigen::Vector3d m_suppliedCamPosition;
        bool m_cameraIsForward;
        /// @}

        bool m_calibComplete = false;
        /// @name Output
        /// @{
        util::Angle m_imuYaw = 0 * util::radians;
        Eigen::Isometry3d m_cameraPose = Eigen::Isometry3d::Identity();
        Eigen::Isometry3d m_rTi = Eigen::Isometry3d::Identity();
        /// @}
    };

    /// A standalone function that looks at the camera and IMUs in a tracking
    /// system to determine whether all calibration is complete.
    bool isRoomCalibrationComplete(TrackingSystem const &sys);
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_RoomCalibration_h_GUID_D53BD552_47D4_4390_C6AC_C6819180D4AF
