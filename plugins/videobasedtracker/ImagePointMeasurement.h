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

#ifndef INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183
#define INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183

// Internal Includes
#include "VideoJacobian.h"
#include "ProjectPoint.h"
#include <osvr/Kalman/FlexibleKalmanBase.h>
#include <osvr/Kalman/PureVectorState.h>
#include <osvr/Kalman/AugmentedState.h>
#include <osvr/Kalman/PoseState.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    using AugmentedStateWithBeacon =
        kalman::AugmentedState<kalman::pose_externalized_rotation::State,
                               kalman::PureVectorState<3>>;
    struct CameraModel {
        Eigen::Vector2d principalPoint;
        double focalLength;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
    /// Measurement class for auto-calibrating Kalman filter in video-based
    /// tracker.
    class ImagePointMeasurement {
      public:
        static const kalman::types::DimensionType DIMENSION = 2;
        using Vector = kalman::types::Vector<DIMENSION>;
        using SquareMatrix = kalman::types::SquareMatrix<DIMENSION>;
        using State = AugmentedStateWithBeacon;
        using Jacobian =
            kalman::types::Matrix<DIMENSION,
                                  kalman::types::Dimension<State>::value>;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        explicit ImagePointMeasurement(CameraModel const &cam) : m_cam(cam) {}

        /// Updates some internal cached partial solutions.
        void updateFromState(State const &state) {
            // 3d position of beacon
            m_beacon = state.b().stateVector();
            m_rot = state.a().getCombinedQuaternion().toRotationMatrix();
            m_rotatedObjPoint = m_rot * m_beacon;
            m_rotatedTranslatedPoint =
                m_rotatedObjPoint + state.a().getPosition();
            m_xlate = state.a().getPosition();
        }
        Vector getResidual(State const &state) const {
            // 3d position of beacon
            Eigen::Vector3d beacon = state.b().stateVector();
            Eigen::Vector2d predicted = projectPoint(
                state.a().getPosition(), state.a().getCombinedQuaternion(),
                m_cam.focalLength, m_cam.principalPoint,
                state.b().stateVector());
            return m_measurement - predicted;
        }

        void setMeasurement(Vector const &m) { m_measurement = m; }
        Eigen::Matrix<double, 2, 3> getBeaconJacobian() const {
            auto v1 = m_rot(0, 2) * m_beacon[2] + m_rot(0, 1) * m_beacon[1] +
                      m_beacon[0] * m_rot(0, 0) + m_xlate[0];
            auto v2 = m_beacon[2] * m_rot(2, 2) + m_beacon[1] * m_rot(2, 1) +
                      m_beacon[0] * m_rot(2, 0) + m_xlate[2];
            auto v3 = 1 / (v2 * v2);
            auto v4 = 1 / v2;
            auto v5 = m_rot(1, 2) * m_beacon[2] + m_beacon[1] * m_rot(1, 1) +
                      m_beacon[0] * m_rot(1, 0) + m_xlate[1];
            Eigen::Matrix<double, 2, 3> ret;
            ret << m_rot(0, 0) * v4 * m_cam.focalLength -
                       v1 * m_rot(2, 0) * v3 * m_cam.focalLength,
                m_rot(0, 1) * v4 * m_cam.focalLength -
                    v1 * m_rot(2, 1) * v3 * m_cam.focalLength,
                m_rot(0, 2) * v4 * m_cam.focalLength -
                    v1 * m_rot(2, 2) * v3 * m_cam.focalLength,
                m_rot(1, 0) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 0) * v3 * m_cam.focalLength,
                m_rot(1, 1) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 1) * v3 * m_cam.focalLength,
                m_rot(1, 2) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 2) * v3 * m_cam.focalLength;
            return ret;
        }
        Jacobian getJacobian(State const &state) const {
            Jacobian ret;
            ret <<
                // with respect to change in x or y
                Eigen::Matrix2d::Identity() *
                    (m_cam.focalLength / m_rotatedTranslatedPoint.z()),
                // with respect to change in z
                -m_rotatedTranslatedPoint.head<2>() * m_cam.focalLength /
                    (m_rotatedTranslatedPoint.z() *
                     m_rotatedTranslatedPoint.z()),
                // with respect to change in incremental rotation, @todo
                Eigen::Matrix<double, 2, 3>::Zero(),
                // with respect to change in linear/angular velocity
                Eigen::Matrix<double, 2, 6>::Zero(),
                // with respect to change in beacon position, @todo

                getBeaconJacobian();
#if 0
            ret << nonzero.topLeftCorner<2, 6>(),
                Eigen::Matrix<double, 2, 6>::Zero(),
                nonzero.topRightCorner<2, 3>();
#endif
            return ret;
        }

        SquareMatrix getCovariance(State &state) const {
            /// @todo make this better, perhaps state dependent.
            return Vector::Constant(2.0).asDiagonal();
        }

      private:
        Vector m_measurement;
        CameraModel m_cam;
        Eigen::Vector3d m_beacon;
        Eigen::Vector3d m_rotatedObjPoint;
        Eigen::Vector3d m_rotatedTranslatedPoint;
        Eigen::Vector3d m_xlate;
        Eigen::Matrix3d m_rot;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183
