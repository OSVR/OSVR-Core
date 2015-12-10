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

#ifndef INCLUDED_AngularVelocityMeasurement_h_GUID_73F00F72_643C_45BC_C0BC_291214F14CF3
#define INCLUDED_AngularVelocityMeasurement_h_GUID_73F00F72_643C_45BC_C0BC_291214F14CF3

// Internal Includes
#include "FlexibleKalmanBase.h"
#include "PoseState.h"
#include "OrientationState.h"
#include "ExternalQuaternion.h"
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    class AngularVelocityBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementDiagonalMatrix = types::DiagonalMatrix<DIMENSION>;
        AngularVelocityBase(MeasurementVector const &vel,
                            MeasurementVector const &variance)
            : m_measurement(vel), m_covariance(variance.asDiagonal()) {}

        template <typename State>
        MeasurementDiagonalMatrix const &getCovariance(State const &) {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can `.angularVelocity()`
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            const MeasurementVector residual =
                m_measurement - s.angularVelocity();
            return residual;
        }

        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(MeasurementVector const &vel) {
            m_measurement = vel;
        }

      private:
        MeasurementVector m_measurement;
        MeasurementDiagonalMatrix m_covariance;
    };

    /// This is the subclass of AngularVelocityBase: only explicit
    /// specializations, and on state types.
    template <typename StateType> class AngularVelocityMeasurement;

    /// AngularVelocityMeasurement with a pose_externalized_rotation::State
    template <>
    class AngularVelocityMeasurement<pose_externalized_rotation::State>
        : public AngularVelocityBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AngularVelocityBase;

        AngularVelocityMeasurement(MeasurementVector const &vel,
                                   MeasurementVector const &variance)
            : Base(vel, variance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &) const {
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.topRightCorner<3, 3>() = types::SquareMatrix<3>::Identity();
            return ret;
        }
    };

    /// AngularVelocityMeasurement with a orient_externalized_rotation::State
    /// The code is in fact identical except for the state types, due to a
    /// coincidence of how the state vectors are arranged.
    template <>
    class AngularVelocityMeasurement<orient_externalized_rotation::State>
        : public AngularVelocityBase {
      public:
        using State = orient_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AngularVelocityBase;

        AngularVelocityMeasurement(MeasurementVector const &vel,
                                   MeasurementVector const &variance)
            : Base(vel, variance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &) const {
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.topRightCorner<3, 3>() = types::SquareMatrix<3>::Identity();
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_AngularVelocityMeasurement_h_GUID_73F00F72_643C_45BC_C0BC_291214F14CF3
