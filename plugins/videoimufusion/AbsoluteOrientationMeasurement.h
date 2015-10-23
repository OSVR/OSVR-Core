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

#ifndef INCLUDED_AbsoluteOrientationMeasurement_h_GUID_71285DD8_A6F1_47A8_4B2E_B10171C91248
#define INCLUDED_AbsoluteOrientationMeasurement_h_GUID_71285DD8_A6F1_47A8_4B2E_B10171C91248

// Internal Includes
#include "PoseState.h"

// Library/third-party includes
#include <Eigen/Geometry>

// Standard includes
// - none

namespace osvr {
namespace kalman {
    class AbsoluteOrientationBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 4;
        using MeasurementVector = types::SquareMatrix<DIMENSION>;
        AbsoluteOrientationBase(
            Eigen::Quaterniond const &quat,
            types::SquareMatrix<DIMENSION> const &covariance)
            : m_measurement(quat), m_covariance(covariance) {}

        template <typename State>
        MeasurementVector getCovariance(State const &) {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can getCombinedQuaternion()
        template <typename State>
        MeasurementVector getResidual(State const &s) {
            Eigen::Quaterniond prediction = s.getCombinedQuaternion();
            return MeasurementVector(prediction * m_measurement.conjugate());
        }

      protected:
        Eigen::Quaterniond const &measurement() const { return m_measurement; }

      private:
        Eigen::Quaterniond m_measurement;
        types::SquareMatrix<DIMENSION> m_covariance;
    };
    template <typename StateType> class AbsoluteOrientationMeasurement;
    template <>
    class AbsoluteOrientationMeasurement<pose_externalized_rotation::State>
        : public AbsoluteOrientationBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AbsoluteOrientationBase;

        AbsoluteOrientationMeasurement(
            Eigen::Quaterniond const &quat,
            types::SquareMatrix<DIMENSION> const &covariance)
            : Base(quat, covariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION> getJacobian(State const &s) {
            using namespace pose_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<DIMENSION, 3>(0, 3) = external_quat::jacobian(
                incrementalOrientation(s.stateVector()));
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_AbsoluteOrientationMeasurement_h_GUID_71285DD8_A6F1_47A8_4B2E_B10171C91248
