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

// Library/third-party includes
#ifdef OSVR_FPE
#include <FPExceptionEnabler.h>
#endif

// Standard includes
// - none

using osvr::util::time::duration;
using osvr::util::fromQuat;

void VideoIMUFusion::RunningData::handleIMUReport(
    const OSVR_TimeValue &timestamp, const OSVR_OrientationReport &report) {
    /// Right now, just accepting the orientation report as it is. This
    /// does not correct for gyro drift.
    m_orientation = fromQuat(report.rotation);

#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif

    if (preReport(timestamp)) {
        m_imuMeas.setMeasurement(fromQuat(report.rotation));
        m_filter.correct(m_imuMeas);
    }
}
void VideoIMUFusion::RunningData::handleVideoTrackerReport(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report) {
    Eigen::Isometry3d roomPose = takeCameraPoseToRoom(report.pose);

#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif
    if (preReport(timestamp)) {
        m_cameraMeas.setMeasurement(roomPose.translation(),
                                    Eigen::Quaterniond(roomPose.rotation()));
        Eigen::Quaterniond(roomPose.rotation());
        m_filter.correct(m_cameraMeas);
#if 0
                        OSVR_DEV_VERBOSE(
                            "State: "
                            << m_filter.state().stateVector().transpose() << "\n"
                            << "Quat: "
                            << m_filter.state().getQuaternion().coeffs().transpose()
                            << "\n"
                               "Error:\n"
                            << m_filter.state().errorCovariance());
#endif
    }
}

/// Returns true if we succeeded and can filter in some data.
bool VideoIMUFusion::RunningData::preReport(const OSVR_TimeValue &timestamp) {
    auto dt = duration(timestamp, m_last);
    if (dt > 0) {
        m_last = timestamp;
        // only predict if time has moved forward
        m_filter.predict(dt);
    }
    // Can always correct though.
    return true;
}
