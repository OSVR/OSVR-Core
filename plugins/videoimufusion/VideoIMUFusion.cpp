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
#include "VideoIMUFusion.h"
#include "RunningData.h"
#include <osvr/Util/EigenFilters.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/ExtractYaw.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <iostream>

#ifdef OSVR_FPE
#include <FPExceptionEnabler.h>
#endif

using osvr::util::time::duration;
using osvr::util::time::getNow;
namespace ei = osvr::util::eigen_interop;

namespace filters = osvr::util::filters;

/// These are the levels that the velocity must remain below for a given number
/// of consecutive frames before we accept a correlation between the IMU and
/// video as truth.
static const auto LINEAR_VELOCITY_CUTOFF = 0.2;
static const auto ANGULAR_VELOCITY_CUTOFF = 1.e-4;

/// The number of low-velocity frames required
static const std::size_t REQUIRED_SAMPLES = 10;

/// The distance from the camera that we want to encourage users to move within
/// for best initial startup. Provides the best view of beacons for initial
/// start of autocalibration.
static const auto NEAR_MESSAGE_CUTOFF = 0.3;

VideoIMUFusion::VideoIMUFusion(VideoIMUFusionParams const &params)
    : m_params(params), m_roomCalib(Eigen::Isometry3d::Identity()) {
    enterCameraPoseAcquisitionState();
}
VideoIMUFusion::~VideoIMUFusion() = default;

Eigen::Matrix<double, 12, 12> const &
VideoIMUFusion::getErrorCovariance() const {
    BOOST_ASSERT_MSG(running(),
                     "Only valid if fusion is in the running state!");
    return m_runningData->getErrorCovariance();
}

void VideoIMUFusion::enterRunningState(
    Eigen::Isometry3d const &rTc, const OSVR_TimeValue &timestamp,
    const OSVR_PoseReport &report, const OSVR_OrientationState &orientation) {
#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif
    m_rTc = rTc;
    std::cout << "\nVideo-IMU fusion: Camera pose acquired, entering normal "
                 "run mode!\n";
    std::cout << "Camera is located in the room at roughly "
              << m_rTc.translation().transpose() << std::endl;

    if (m_params.cameraIsForward) {
        auto yaw = osvr::util::extractYaw(Eigen::Quaterniond(m_rTc.rotation()));
        Eigen::AngleAxisd correction(-yaw, Eigen::Vector3d::UnitY());
        m_roomCalib = Eigen::Isometry3d(correction);
    }

    m_state = State::Running;
    m_runningData.reset(new VideoIMUFusion::RunningData(
        m_params, m_rTc, orientation, report.pose, timestamp));
    /// @todo should we just let it hang around instead of releasing memory in a
    /// callback?
    m_startupData.reset();
    ei::map(m_camera) = m_roomCalib * m_rTc;
}

void VideoIMUFusion::handleIMUData(const OSVR_TimeValue &timestamp,
                                   const OSVR_OrientationReport &report) {
    if (m_state != State::Running) {
        // Just report the orientation with height added on.
        m_lastPose.rotation = report.rotation;
        ei::map(m_lastPose).translation() =
            Eigen::Vector3d::UnitY() * m_params.eyeHeight;
        m_lastTime = timestamp;
        return;
    }
    m_runningData->handleIMUReport(timestamp, report);

    // send a pose report
    updateFusedOutput(timestamp);
}
void VideoIMUFusion::handleIMUVelocity(const OSVR_TimeValue &timestamp,
                                       const Eigen::Vector3d &angVel) {
    if (m_state != State::Running) {
        return;
    }
    m_runningData->handleIMUVelocity(timestamp, angVel);
    // send a pose report
    updateFusedOutput(timestamp);
}

void VideoIMUFusion::updateFusedOutput(const OSVR_TimeValue &timestamp) {
    Eigen::Isometry3d initialPose =
        Eigen::Translation3d(m_runningData->getPosition() +
                             Eigen::Vector3d::UnitY() * m_params.eyeHeight) *
        m_runningData->getOrientation();
    Eigen::Isometry3d transformed = m_roomCalib * initialPose;
    ei::map(m_lastPose).rotation() = Eigen::Quaterniond(transformed.rotation());
    ei::map(m_lastPose).translation() =
        Eigen::Vector3d(transformed.translation());
    m_lastTime = timestamp;
}

void VideoIMUFusion::handleVideoTrackerDataWhileRunning(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report) {
    assert(running() &&
           "handleVideoTrackerDataWhileRunning() called when not running!");
    // Pass this along to the filter
    m_runningData->handleVideoTrackerReport(timestamp, report);

    updateFusedOutput(timestamp);
    // For debugging, we will output a second sensor that is just the
    // video tracker data re-oriented.
    Eigen::Isometry3d videoPose =
        m_roomCalib * m_runningData->takeCameraPoseToRoom(report.pose);
    ei::map(m_reorientedVideo) = videoPose;
}

class VideoIMUFusion::StartupData {
  public:
    StartupData()
        : last(getNow()), positionFilter(filters::one_euro::Params{}),
          orientationFilter(filters::one_euro::Params{}) {}
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    void handleReport(const OSVR_TimeValue &timestamp,
                      const OSVR_PoseReport &report,
                      const OSVR_OrientationState &orientation) {
        auto dt = duration(timestamp, last);
        if (dt <= 0) {
            dt = 1; // in case of weirdness, avoid divide by zero.
        }
        // Pose of tracked device (in camera space) is cTd
        // orientation is rTd or iTd: tracked device in IMU space (aka room
        // space, modulo yaw)
        // rTc is camera in room space (what we want to find), so we can take
        // camera-reported cTd, perform rTc * cTd, and end up with rTd with
        // position...
        Eigen::Isometry3d rTc = Eigen::Isometry3d(ei::map(orientation).quat()) *
                                ei::map(report.pose).transform().inverse();
        positionFilter.filter(dt, rTc.translation());
        orientationFilter.filter(dt, Eigen::Quaterniond(rTc.rotation()));
        auto linearVel = positionFilter.getDerivativeMagnitude();
        auto angVel = orientationFilter.getDerivativeMagnitude();
        // std::cout << "linear " << linearVel << " ang " << angVel << "\n";
        if (linearVel < LINEAR_VELOCITY_CUTOFF &&
            angVel < ANGULAR_VELOCITY_CUTOFF) {
            // OK, velocity within bounds
            if (reports == 0) {
                std::cout
                    << "Video-IMU fusion: Hold still, measuring camera pose";
            }
            std::cout << "." << std::flush;
            ++reports;
        } else {
            // reset the count if movement too fast.
            if (reports > 0) {
                /// put an end to the dots
                std::cout << std::endl;
            }
            reports = 0;
            if (!toldToMoveCloser &&
                osvrVec3GetZ(&report.pose.translation) > NEAR_MESSAGE_CUTOFF) {
                std::cout
                    << "\n\nNOTE: For best results, during tracker/server "
                       "startup, hold your head/HMD still closer than "
                    << NEAR_MESSAGE_CUTOFF
                    << " meters from the tracking camera for a few "
                       "seconds, then rotate slowly in all directions.\n\n"
                    << std::endl;
                toldToMoveCloser = true;
            } else if (toldToMoveCloser && !toldDistanceIsGood &&
                       osvrVec3GetZ(&report.pose.translation) <
                           0.9 * NEAR_MESSAGE_CUTOFF) {
                std::cout
                    << "\nThat distance looks good, hold it right there.\n"
                    << std::endl;
                toldDistanceIsGood = true;
            }
        }
        last = timestamp;
        if (finished()) {
            /// put an end to the dots
            std::cout << "\n" << std::endl;
        }
    }

    bool finished() const { return reports >= REQUIRED_SAMPLES; }

    Eigen::Isometry3d getRoomToCamera() const {
        Eigen::Isometry3d ret;
        ret.fromPositionOrientationScale(positionFilter.getState(),
                                         orientationFilter.getState(),
                                         Eigen::Vector3d::Constant(1));
        return ret;
    }

  private:
    std::size_t reports = 0;
    OSVR_TimeValue last;

    bool toldToMoveCloser = false;
    bool toldDistanceIsGood = false;

    filters::OneEuroFilter<Eigen::Vector3d> positionFilter;
    filters::OneEuroFilter<Eigen::Quaterniond> orientationFilter;
};

void VideoIMUFusion::enterCameraPoseAcquisitionState() {
    m_startupData.reset(new VideoIMUFusion::StartupData);
    m_state = State::AcquiringCameraPose;
}

void VideoIMUFusion::handleVideoTrackerDataDuringStartup(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report,
    const OSVR_OrientationState &orientation) {
    assert(!running() &&
           "handleVideoTrackerDataDuringStartup() called when running!");
    m_startupData->handleReport(timestamp, report, orientation);
    if (m_startupData->finished()) {
        enterRunningState(m_startupData->getRoomToCamera(), timestamp, report,
                          orientation);
    }
}
