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

#ifndef INCLUDED_AbsolutePositionMeasurement_h_GUID_F2854FEA_1E84_479E_1B5D_A302104DBF47
#define INCLUDED_AbsolutePositionMeasurement_h_GUID_F2854FEA_1E84_479E_1B5D_A302104DBF47

// Internal Includes
#include "FlexibleKalmanBase.h"
#include "PoseState.h"
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {

    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class AbsolutePositionBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3; // 3 position
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementDiagonalMatrix = types::DiagonalMatrix<DIMENSION>;
        using MeasurementMatrix = types::SquareMatrix<DIMENSION>;
        AbsolutePositionBase(MeasurementVector const &pos,
                             MeasurementVector const &variance)
            : m_pos(pos), m_covariance(variance.asDiagonal()) {}

        template <typename State>
        MeasurementMatrix getCovariance(State const &) const {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can getPosition()
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            MeasurementVector residual = m_pos - s.getPosition();
            return residual;
        }

        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(MeasurementVector const &pos) { m_pos = pos; }

      private:
        MeasurementVector m_pos;
        MeasurementDiagonalMatrix m_covariance;
    };

    /// This is the subclass of AbsolutePositionBase: only explicit
    /// specializations,
    /// and on state types.
    template <typename StateType> class AbsolutePositionMeasurement;

    /// AbsolutePositionMeasurement with a pose_externalized_rotation::State
    template <>
    class AbsolutePositionMeasurement<pose_externalized_rotation::State>
        : public AbsolutePositionBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AbsolutePositionBase;
        using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
        AbsolutePositionMeasurement(MeasurementVector const &pos,
                                    MeasurementVector const &variance)
            : Base(pos, variance), m_jacobian(Jacobian::Zero()) {
            m_jacobian.block<3, 3>(0, 0) = types::SquareMatrix<3>::Identity();
        }

        types::Matrix<DIMENSION, STATE_DIMENSION> const &
        getJacobian(State const &) const {
            return m_jacobian;
        }

      private:
        types::Matrix<DIMENSION, STATE_DIMENSION> m_jacobian;
    };
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_AbsolutePositionMeasurement_h_GUID_F2854FEA_1E84_479E_1B5D_A302104DBF47
