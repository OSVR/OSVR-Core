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
#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif

    if (preReport(timestamp)) {
        m_imuMeas.setMeasurement(ei::map(report.rotation));
        osvr::kalman::correct(state(), processModel(), m_imuMeas);
    }
}
void VideoIMUFusion::RunningData::handleIMUVelocity(
    const OSVR_TimeValue &timestamp, const Eigen::Vector3d &angVel) {
#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif

    if (preReport(timestamp)) {

#if 0
        static int s = 0;
        if (s == 0) {
            static const Eigen::IOFormat fmt(3, 0, ", ", ";\n", "", "", "[",
                                             "]");
            OSVR_DEV_VERBOSE(
                "\nprediction: "
                << state().getAngularVelocity().transpose().format(fmt)
                << "\nMeasurement: " << angVel.transpose().format(fmt)
                << "\nVariance: "
                << state()
                       .errorCovariance()
                       .diagonal()
                       .tail<3>()
                       .transpose()
                       .format(fmt));
        }
        s = (s + 1) % 100;
#endif

        m_imuMeasVel.setMeasurement(angVel);
        osvr::kalman::correct(state(), processModel(), m_imuMeasVel);
    }
}

void VideoIMUFusion::RunningData::handleVideoTrackerReport(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report) {
    Eigen::Isometry3d roomPose = takeCameraPoseToRoom(report.pose);

#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif
    Eigen::Quaterniond orientation(roomPose.rotation());
    auto oriChange = state().getQuaternion().angularDistance(orientation);
    if (std::abs(oriChange) > M_PI / 2.) {
        OSVR_DEV_VERBOSE("Throwing out a bad video pose");
        return;
    }
    if (preReport(timestamp)) {
        m_cameraMeasPos.setMeasurement(roomPose.translation());
        osvr::kalman::correct(state(), processModel(), m_cameraMeasPos);
        m_cameraMeasOri.setMeasurement(orientation);
        osvr::kalman::correct(state(), processModel(), m_cameraMeasOri);

#if 0
    OSVR_DEV_VERBOSE(
        "State: " << state().stateVector().transpose() << "\n"
                  << "Quat: "
                  << state().getQuaternion().coeffs().transpose()
                  << "\n"
                     "Error:\n"
                  << state().errorCovariance());
#endif
    }
}

/// Returns true if we succeeded and can filter in some data.
bool VideoIMUFusion::RunningData::preReport(const OSVR_TimeValue &timestamp) {
    auto dt = duration(timestamp, m_last);
    if (dt > 0) {
        m_last = timestamp;
        // only predict if time has moved forward
        osvr::kalman::predict(state(), processModel(), dt);
    }
    // Can always correct though.
    /// @todo this is a crude way of handing video timestamps in the past.
    /// Video tracker data is usually timestamped "in the past" - ideally would
    /// probably roll back the state to the next earlier report then apply the
    /// measurements again in order.
    /// Right now, we're just not advancing time at all for such "out of order"
    /// reports, meaning that they're being incorporated as if they were roughly
    /// 5ms newer than they actually are.
    return true;
}
