/** @file
    @brief Implementation of RunningData methods that compile quickly.

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

static const double InitialStateError[] = {
    1., 1., 1., 10., 10., 10., 100., 100., 100., 1000., 1000., 1000.};
#if 0
static const double IMUError = 1.0E-8;
static const double IMUErrorVector[] = {IMUError, IMUError * 5., IMUError};
static const double CameraOriError = 1.0E-1;
static const double CameraOrientationError[] = {CameraOriError, CameraOriError,
                                                CameraOriError};
static const double CameraPosError = 3.0E-4;
static const double CameraPositionError[] = {CameraPosError, CameraPosError,
                                             CameraPosError * 0.1};

static const double PositionNoiseAutocorrelation = 0.01;
static const double OrientationNoiseAutocorrelation = 0.1;

static const double VelocityDamping = .01;
#endif

using osvr::kalman::types::Vector;
namespace ei = osvr::util::eigen_interop;

VideoIMUFusion::RunningData::RunningData(
    VideoIMUFusionParams const &params, Eigen::Isometry3d const &cTr,
    OSVR_OrientationState const &initialIMU, OSVR_PoseState const &initialVideo,
    OSVR_TimeValue const &lastTS)
    : m_processModel(params.damping, params.positionNoise, params.oriNoise),
      m_state(), m_imuMeas(ei::map(initialIMU),
                           Vector<3>::Constant(params.imuOriVariance)),
      m_imuMeasVel(Vector<3>::Zero(),
                   Vector<3>::Constant(params.imuAngVelVariance)),
      m_cameraMeasOri(Eigen::Quaterniond::Identity(),
                      Vector<3>::Constant(params.videoOriVariance)),
      m_cameraMeasPos(Vector<3>::Zero(),
                      Vector<3>::Constant(params.videoPosVariance)),
      m_cTr(cTr), m_last(lastTS) {

#ifdef OSVR_FPE
    FPExceptionEnabler fpe;
#endif
    Eigen::Isometry3d roomPose = takeCameraPoseToRoom(initialVideo);
    osvr::kalman::types::DimVector<FilterState> initialState =
        osvr::kalman::types::DimVector<FilterState>::Zero();
    using namespace osvr::kalman::pose_externalized_rotation;

    position(initialState) = roomPose.translation();
    state().setStateVector(initialState);
    state().setQuaternion(Eigen::Quaterniond(roomPose.rotation()));
    state().setErrorCovariance(Vector<12>(InitialStateError).asDiagonal());
}
