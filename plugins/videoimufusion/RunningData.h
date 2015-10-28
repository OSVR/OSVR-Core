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

// Internal Includes
#include "VideoIMUFusion.h"
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/TimeValue.h>

#include "FlexibleKalmanFilter.h"
#include "PoseDampedConstantVelocity.h"
#include "AbsoluteOrientationMeasurement.h"
#include "AbsolutePoseMeasurement.h"

#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

using ProcessModel = osvr::kalman::PoseDampedConstantVelocityProcessModel;
using FilterState = ProcessModel::State;
using AbsoluteOrientationMeasurement =
    osvr::kalman::AbsoluteOrientationMeasurement<FilterState>;
using AbsolutePoseMeasurement =
    osvr::kalman::AbsolutePoseMeasurement<FilterState>;
using Filter = osvr::kalman::FlexibleKalmanFilter<ProcessModel>;

using osvr::util::fromPose;
using osvr::util::fromQuat;
using osvr::util::vecMap;
using osvr::util::time::getNow;
using osvr::util::time::duration;

static const double InitialStateError[] = {1., 1., 1., 1., 1., 1.,
                                           1., 1., 1., 1., 1., 1.};
static const double IMUError[] = {1., 1.5, 1.};
static const double CameraOrientationError[] = {1.1, 1.1, 1.1};
static const double CameraPositionError[] = {1., 1., 1.};
using osvr::kalman::types::Vector;
class VideoIMUFusion::RunningData {
  public:
    RunningData(Eigen::Isometry3d const &cTr,
                OSVR_OrientationState const &initialIMU,
                OSVR_PoseState const &initialVideo,
                OSVR_TimeValue const &lastPosition,
                OSVR_TimeValue const &lastIMU)
        : m_filter(), m_cTr(cTr), m_orientation(fromQuat(initialIMU)),
          m_last(lastPosition) {

#ifdef OSVR_FPE
        FPExceptionEnabler fpe;
#endif
        Eigen::Isometry3d roomPose = takeCameraPoseToRoom(initialVideo);
        osvr::kalman::types::DimVector<FilterState> initialState =
            osvr::kalman::types::DimVector<FilterState>::Zero();
        using namespace osvr::kalman::pose_externalized_rotation;
        position(initialState) = roomPose.translation();
        m_filter.state().setStateVector(initialState);
        m_filter.state().setQuaternion(Eigen::Quaterniond(roomPose.rotation()));
        m_filter.state().setErrorCovariance(
            Vector<12>(InitialStateError).asDiagonal());

        m_filter.processModel().noiseAutocorrelation.head<3>() *= 0.001;
        m_filter.processModel().noiseAutocorrelation.tail<3>() *= 0.01;

        // Very crudely set up some error estimates.
        using osvr::kalman::external_quat::vecToQuat;

        // This line produces FPE
        // Eigen::Quaterniond IMUErrorQuat =
        // vecToQuat(Vector<3>::Map(IMUError));

        // This line is OK but not very good
        // Eigen::Vector4d quatError(1, 1, 1, 1); m_imuError = quatError;

        Eigen::Quaterniond imuQuatError = vecToQuat(Vector<3>::Map(IMUError));
        m_imuError = imuQuatError.coeffs();

        m_cameraError.head<3>() = Vector<3>::Map(CameraPositionError);
        Eigen::Quaterniond cameraQuatError =
            vecToQuat(Vector<3>::Map(CameraOrientationError));
        m_cameraError.tail<4>() = cameraQuatError.coeffs();
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    void handleIMUReport(const OSVR_TimeValue &timestamp,
                         const OSVR_OrientationReport &report) {
        /// Right now, just accepting the orientation report as it is. This
        /// does not correct for gyro drift.
        m_orientation = fromQuat(report.rotation);

#ifdef OSVR_FPE
        FPExceptionEnabler fpe;
#endif

        if (preReport()) {
            AbsoluteOrientationMeasurement meas{fromQuat(report.rotation),
                                                m_imuError.asDiagonal()};
            m_filter.correct(meas);
        }
    }
    void handleVideoTrackerReport(const OSVR_TimeValue &timestamp,
                                  const OSVR_PoseReport &report) {
        Eigen::Isometry3d roomPose = takeCameraPoseToRoom(report.pose);

#ifdef OSVR_FPE
        FPExceptionEnabler fpe;
#endif
        if (preReport()) {
            Eigen::Quaterniond ori(roomPose.rotation());
            Eigen::Vector3d pos(roomPose.translation());
            AbsolutePoseMeasurement meas{pos, ori, m_cameraError.asDiagonal()};
            m_filter.correct(meas);
            OSVR_DEV_VERBOSE(
                "State: "
                << m_filter.state().stateVector().transpose() << "\n"
                << "Quat: "
                << m_filter.state().getQuaternion().coeffs().transpose()
                << "\n"
                   "Error:\n"
                << m_filter.state().errorCovariance());
        }
    }

    /// Returns true if we succeeded and can filter in some data.
    bool preReport() {
        auto timestamp = getNow();
        auto dt = duration(timestamp, m_last);
        if (dt <= 0) {
            return false;
        }

        m_last = timestamp;
        m_filter.predict(dt);
        return true;
    }
    Eigen::Quaterniond getOrientation() const {
        return m_filter.state().getQuaternion();
    }
    Eigen::Vector3d getPosition() const {
        return m_filter.state().getPosition();
    }

    Eigen::Isometry3d takeCameraPoseToRoom(OSVR_PoseState const &pose) {
        return m_cTr * fromPose(pose);
    }

  private:
    Filter m_filter;
    Vector<4> m_imuError;
    Vector<7> m_cameraError;
    const Eigen::Isometry3d m_cTr;
    Eigen::Quaterniond m_orientation;
    OSVR_TimeValue m_last;
};

#endif // INCLUDED_RunningData_h_GUID_6B3479E5_9D56_4BA9_DEC0_84AF53842168
