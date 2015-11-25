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

        explicit ImagePointMeasurement(CameraModel const &cam) : m_cam(cam) {}
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

        Jacobian getJacobian(State const &state) const {
            return getVideoJacobian(
                state.a().getPosition(),
                kalman::pose_externalized_rotation::incrementalOrientation(
                    state.a().stateVector()),
                state.a().getCombinedQuaternion(), m_cam.focalLength,
                state.b().stateVector());
        }

        SquareMatrix getCovariance(State & state) const {
            /// @todo make this better, perhaps state dependent.
            return Vector::Constant(2.0).asDiagonal();
        }

      private:
        Vector m_measurement;
        CameraModel m_cam;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183
