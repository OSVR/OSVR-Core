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

VideoIMUFusion::VideoIMUFusion(VideoIMUFusionParams const &params)
    : m_params(params) {
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
    Eigen::Isometry3d const &cTr, const OSVR_TimeValue &timestamp,
    const OSVR_PoseReport &report, const OSVR_OrientationState &orientation) {
#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif
    m_cTr = cTr;
    ei::map(m_camera) = cTr;
    std::cout << "Camera is located in the room at roughly "
              << m_cTr.translation().transpose() << std::endl;
    m_state = State::Running;
    m_runningData.reset(new VideoIMUFusion::RunningData(
        m_params, cTr, orientation, report.pose, timestamp));
    /// @todo should we just let it hang around instead of releasing memory in a
    /// callback?
    m_startupData.reset();
}

void VideoIMUFusion::handleIMUData(const OSVR_TimeValue &timestamp,
                                   const OSVR_OrientationReport &report) {
    if (m_state != State::Running) {
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
    ei::map(m_lastFusion).rotation() = m_runningData->getOrientation();
    ei::map(m_lastFusion).translation() = m_runningData->getPosition();
    m_lastFusionTime = timestamp;
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
    auto videoPose = m_runningData->takeCameraPoseToRoom(report.pose);
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
        // tranform from camera to tracked device is dTc
        // orientation is dTr: room to tracked device
        // cTr is room to camera, so we can take camera-reported dTc * cTr and
        // get dTr with position...
        Eigen::Isometry3d cTr = ei::map(report.pose).transform().inverse() *
                                Eigen::Isometry3d(ei::map(orientation).quat());
        positionFilter.filter(dt, cTr.translation());
        orientationFilter.filter(dt, Eigen::Quaterniond(cTr.rotation()));
        ++reports;
        last = timestamp;
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
    static const std::size_t REQUIRED_SAMPLES = 10;
    std::size_t reports = 0;
    OSVR_TimeValue last;

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
