/** @file
    @brief Header

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

#ifndef INCLUDED_RunningData_h_GUID_6B3479E5_9D56_4BA9_DEC0_84AF53842168
#define INCLUDED_RunningData_h_GUID_6B3479E5_9D56_4BA9_DEC0_84AF53842168

#ifdef OSVR_VIDEOIMUFUSION_VERBOSE
#include <iostream>
#include <osvr/Util/EigenCoreGeometry.h>
template <typename T>
inline void dumpKalmanDebugOuput(const char name[], const char expr[],
                                 T const &value) {
    std::cout << "\n(Kalman Debug Output) " << name << " [" << expr << "]:\n"
              << value << std::endl;
}

#define OSVR_KALMAN_DEBUG_OUTPUT(Name, Value)                                  \
    dumpKalmanDebugOuput(Name, #Value, Value)

#endif // OSVR_VIDEOIMUFUSION_VERBOSE

// Internal Includes
#include "VideoIMUFusion.h"
#include "FusionParams.h"
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/TimeValue.h>

#include <osvr/Kalman/PoseDampedConstantVelocity.h>
#if 0
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AbsolutePositionMeasurement.h>
#include <osvr/Kalman/AngularVelocityMeasurement.h>
#endif

#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

using ProcessModel = osvr::kalman::PoseDampedConstantVelocityProcessModel;
using FilterState = ProcessModel::State;
#if 0
using AbsoluteOrientationMeasurement =
    osvr::kalman::AbsoluteOrientationMeasurement<FilterState>;
using AbsolutePositionMeasurement =
    osvr::kalman::AbsolutePositionMeasurement<FilterState>;
using AngularVelocityMeasurement =
    osvr::kalman::AngularVelocityMeasurement<FilterState>;
#endif
class VideoIMUFusion::RunningData {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    RunningData(VideoIMUFusionParams const &params,
                Eigen::Isometry3d const &cTr,
                OSVR_OrientationState const &initialIMU,
                OSVR_PoseState const &initialVideo,
                OSVR_TimeValue const &lastTS);

    void handleIMUReport(const OSVR_TimeValue &timestamp,
                         const OSVR_OrientationReport &report);
    void handleIMUVelocity(const OSVR_TimeValue &timestamp,
                           const Eigen::Vector3d &angVel);
    void handleVideoTrackerReport(const OSVR_TimeValue &timestamp,
                                  const OSVR_PoseReport &report);

    /// Returns true if we succeeded and can filter in some data.
    bool preReport(const OSVR_TimeValue &timestamp);

    Eigen::Quaterniond getOrientation() const {
        return state().getQuaternion();
    }
    Eigen::Vector3d getPosition() const { return state().getPosition(); }

    Eigen::Isometry3d takeCameraPoseToRoom(OSVR_PoseState const &pose) {
        return m_cTr.linear() * (Eigen::Translation3d(m_cTr.translation()) *
               osvr::util::eigen_interop::map(pose).transform());
    }

    Eigen::Matrix<double, 12, 12> const &getErrorCovariance() const {
        return state().errorCovariance();
    }

  private:
    FilterState &state() { return m_state; }
    FilterState const &state() const { return m_state; }
    ProcessModel &processModel() { return m_processModel; }
    ProcessModel const &processModel() const { return m_processModel; }
    ProcessModel m_processModel;
    FilterState m_state;
#if 0
    AbsoluteOrientationMeasurement m_imuMeas;
    AngularVelocityMeasurement m_imuMeasVel;
    AbsoluteOrientationMeasurement m_cameraMeasOri;
    AbsolutePositionMeasurement m_cameraMeasPos;
#endif
    const Eigen::Isometry3d m_cTr;
    OSVR_TimeValue m_last;
};

#endif // INCLUDED_RunningData_h_GUID_6B3479E5_9D56_4BA9_DEC0_84AF53842168
