/** @file
    @brief Implementation of the RunningData methods that perform the Kalman
   prediction and correction: they are somewhat slow to compile so they're
   separated out here to avoid unnecessary rebuilds.

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
#include "RunningData.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#ifdef OSVR_FPE
#include <FPExceptionEnabler.h>
#endif

// Standard includes
// - none

using osvr::util::time::duration;

namespace ei = osvr::util::eigen_interop;

void VideoIMUFusion::RunningData::handleIMUReport(
    const OSVR_TimeValue &timestamp, const OSVR_OrientationReport &report) {
    state().setQuaternion(ei::map(report.rotation));
}

void VideoIMUFusion::RunningData::handleIMUVelocity(
    const OSVR_TimeValue &timestamp, const Eigen::Vector3d &angVel) {
    Eigen::Matrix<double, 12, 1> x = state().stateVector();
    osvr::kalman::pose_externalized_rotation::angularVelocity(x) = angVel;
    state().setStateVector(x);
}

void VideoIMUFusion::RunningData::handleVideoTrackerReport(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report) {
    Eigen::Isometry3d roomPose = takeCameraPoseToRoom(report.pose);
    Eigen::Matrix<double, 12, 1> x = state().stateVector();
    osvr::kalman::pose_externalized_rotation::position(x) =
        roomPose.translation();
    state().setStateVector(x);
}

/// Returns true if we succeeded and can filter in some data.
bool VideoIMUFusion::RunningData::preReport(const OSVR_TimeValue &timestamp) {
    auto dt = duration(timestamp, m_last);
    if (dt > 0) {
        m_last = timestamp;
    }
    return true;
}
