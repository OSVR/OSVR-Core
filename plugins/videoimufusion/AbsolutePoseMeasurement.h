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

#ifndef INCLUDED_AbsolutePoseMeasurement_h_GUID_A7DDC7F5_CBA1_41AB_B80C_5A9DA444AF48
#define INCLUDED_AbsolutePoseMeasurement_h_GUID_A7DDC7F5_CBA1_41AB_B80C_5A9DA444AF48

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace kalman {
    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class AbsolutePoseBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION =
            7; // 3 position, 4 elements from quat
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementMatrix = types::SquareMatrix<DIMENSION>;
        using Position = types::Vector<3>;
        AbsolutePoseBase(Position const &pos, Eigen::Quaterniond const &quat,
                         types::SquareMatrix<DIMENSION> const &covariance)
            : m_ori(quat), m_covariance(covariance) {}

        template <typename State>
        MeasurementMatrix getCovariance(State const &) const {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can getCombinedQuaternion()
        /// and getPosition()
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            MeasurementVector residual;
            residual.head<3>() = s.getPosition() - m_pos;
            residual.tail<4>() = external_quat::getVec4(
                s.getCombinedQuaternion() * m_ori.conjugate());
            return residual;
        }

      private:
        Position m_pos;
        Eigen::Quaterniond m_ori;
        MeasurementMatrix m_covariance;
    };

    /// This is the subclass of AbsolutePoseBase: only explicit specializations,
    /// and on state types.
    template <typename StateType> class AbsolutePoseMeasurement;

    /// AbsolutePoseMeasurement with a pose_externalized_rotation::State
    template <>
    class AbsolutePoseMeasurement<pose_externalized_rotation::State>
        : public AbsolutePoseBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AbsolutePoseBase;

        AbsolutePoseMeasurement(
            Position const &pos, Eigen::Quaterniond const &quat,
            types::SquareMatrix<DIMENSION> const &covariance)
            : Base(pos, quat, covariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &s) const {
            using namespace pose_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<3, 3>(0, 0) = types::SquareMatrix<3>::Identity();
            ret.block<4, 3>(3, 3) = external_quat::jacobian(
                incrementalOrientation(s.stateVector()));
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_AbsolutePoseMeasurement_h_GUID_A7DDC7F5_CBA1_41AB_B80C_5A9DA444AF48
