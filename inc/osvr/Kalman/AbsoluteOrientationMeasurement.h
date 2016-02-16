/** @file
    @brief Header for measurements of absolute orientation.

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
#include "FlexibleKalmanBase.h"
#include "PoseState.h"
#include "ExternalQuaternion.h"
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenQuatExponentialMap.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class AbsoluteOrientationBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementSquareMatrix = types::SquareMatrix<DIMENSION>;
        AbsoluteOrientationBase(Eigen::Quaterniond const &quat,
                                types::Vector<3> const &emVariance)
            : m_quat(quat), m_covariance(emVariance.asDiagonal()) {}

        template <typename State>
        MeasurementSquareMatrix const &getCovariance(State const &) {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can
        /// `.getCombinedQuaternion()`
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            const Eigen::Quaterniond prediction = s.getCombinedQuaternion();
            const Eigen::Quaterniond residual = m_quat * prediction.inverse();
            // Use the dot product to choose which of the two equivalent
            // quaternions to get the log of for the residual.
            const Eigen::Quaterniond equivalentResidual =
                Eigen::Quaterniond(-(residual.coeffs()));
            auto dot = prediction.dot(residual);
            return dot >= 0 ? util::quat_exp_map(residual).ln()
                            : util::quat_exp_map(equivalentResidual).ln();
        }
        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(Eigen::Quaterniond const &quat) { m_quat = quat; }

        /// Get the block of jacobian that is non-zero: your subclass will have
        /// to put it where it belongs for each particular state type.
        types::Matrix<DIMENSION, 3> getJacobianBlock() const {
            return Eigen::Matrix3d::Identity();
        }

      private:
        Eigen::Quaterniond m_quat;
        MeasurementSquareMatrix m_covariance;
    };

    /// This is the subclass of AbsoluteOrientationBase: only explicit
    /// specializations, and on state types.
    template <typename StateType> class AbsoluteOrientationMeasurement;

    /// AbsoluteOrientationMeasurement with a pose_externalized_rotation::State
    template <>
    class AbsoluteOrientationMeasurement<pose_externalized_rotation::State>
        : public AbsoluteOrientationBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AbsoluteOrientationBase;

        AbsoluteOrientationMeasurement(Eigen::Quaterniond const &quat,
                                       types::Vector<3> const &eulerVariance)
            : Base(quat, eulerVariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &s) const {
            using namespace pose_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<DIMENSION, 3>(0, 3) = Base::getJacobianBlock();
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_AbsoluteOrientationMeasurement_h_GUID_71285DD8_A6F1_47A8_4B2E_B10171C91248
