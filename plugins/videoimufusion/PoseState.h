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

#ifndef INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A
#define INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A

// Internal Includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace kalman {
    namespace pose_externalized_rotation {
        static const std::size_t DIMENSION = 12;
        using StateVector = Eigen::Matrix<double, DIMENSION, 1>;
        using StateVectorBlock3 =
            typename StateVector::template FixedSegmentReturnType<3>::Type;
        using ConstStateVectorBlock3 =
            typename StateVector::template ConstFixedSegmentReturnType<3>::Type;

        using StateVectorBlock6 =
            typename StateVector::template FixedSegmentReturnType<6>::Type;
        using StateSquareMatrix = Eigen::Matrix<double, DIMENSION, DIMENSION>;

        /// @name Accessors to blocks in the state vector.
        /// @{
        inline StateVectorBlock3 position(StateVector &vec) {
            return vec.head<3>();
        }

        inline ConstStateVectorBlock3 position(StateVector const &vec) {
            return vec.head<3>();
        }

        inline StateVectorBlock3 incrementalOrientation(StateVector &vec) {
            return vec.segment<3>(3);
        }
        inline ConstStateVectorBlock3
        incrementalOrientation(StateVector const &vec) {
            return vec.segment<3>(3);
        }

        inline StateVectorBlock3 velocity(StateVector &vec) {
            return vec.segment<3>(6);
        }
        inline ConstStateVectorBlock3 velocity(StateVector const &vec) {
            return vec.segment<3>(6);
        }
        inline StateVectorBlock3 angularVelocity(StateVector &vec) {
            return vec.segment<3>(9);
        }

        inline ConstStateVectorBlock3 angularVelocity(StateVector const &vec) {
            return vec.segment<3>(9);
        }
        /// both translational and angular velocities
        inline StateVectorBlock6 velocities(StateVector &vec) {
            return vec.segment<6>(6);
        }
        /// @}

        inline StateVector applyVelocity(StateVector const &state, double dt) {
// eq. 4.5 in Welch 1996

/// @todo benchmark - assuming for now that the manual small calcuations are
/// faster than the matrix ones.

#if 0
			auto A = StateSquareMatrix::Identity();
			A.block<3, 3>(0, 3) = Eigen::Matrix3d::Identity() * dt;
			A.block<3, 3>(6, 9) = Eigen::Matrix3d::Identity() * dt;

			return A * m_state;
#else
            StateVector ret = state;
            position(ret) += velocity(state) * dt;
            incrementalOrientation(ret) += angularVelocity(state) * dt;
            return ret;
#endif
        }

        inline void dampenVelocities(StateVector &state, double damping,
                                     double dt) {
            auto attenuation = std::pow(damping, dt);
            velocities(state) *= attenuation;
        }

        /// For use in maintaining an "external quaternion" and 3 incremental
        /// orientations, as done by Welch based on earlier work.
        ///
        /// In particular, this function implements equation 6 from a work cited
        /// by Welch,
        /// Azarbayejani, A., & Pentland, A. P. (1995). Recursive estimation of
        /// motion, structure, and focal length. Pattern Analysis and Machine
        /// Intelligence, IEEE Transactions on, 17(6), 562–575.
        /// http://doi.org/10.1109/34.387503
        inline Eigen::Quaterniond
        incrementalOrientationToQuat(StateVector const &state) {
            /// @todo does it matter the order, since they're small angles?
            return Eigen::Quaterniond(
                Eigen::AngleAxisd(state[6], Eigen::Vector3d::UnitX()) *
                Eigen::AngleAxisd(state[7], Eigen::Vector3d::UnitY()) *
                Eigen::AngleAxisd(state[8], Eigen::Vector3d::UnitZ()));
        }

        class State {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            static const std::size_t DIMENSION = 12;

            StateVector &stateVector() { return m_state; }
            StateVector const &stateVector() const { return m_state; }
            void externalizeRotation() {
                /// @todo is just quat multiplication OK here?
                m_orientation =
                    incrementalOrientationToQuat(m_state) * m_orientation;
                incrementalOrientation(m_state) = Eigen::Vector3d::Zero();
            }

          private:
            /// In order: x, y, z, incremental rotations phi (about x), theta
            /// (about
            /// y), psy (about z), then their derivatives in the same order.
            StateVector m_state;
            /// Externally-maintained orientation per Welch 1996
            Eigen::Quaterniond m_orientation;
        };
    } // namespace pose_externalized_rotation

    class PoseConstantVelocityProcessModel {
      public:
        using State = pose_externalized_rotation::State;
        using StateVector = pose_externalized_rotation::StateVector;
        using StateSquareMatrix = pose_externalized_rotation::StateSquareMatrix;
        static const auto DIMENSION = State::DIMENSION;

        /// Set the damping - must be non-negative
        void setDamping(double damping) {
            if (damping >= 0) {
                m_damp = damping;
            }
        }

        /// @return a matrix of dimension n x n. Note that it is real
        /// symmetrical (self-adjoint), so .selfAdjointView<Eigen::Upper>() can
        /// provide useful performance enhancements.
        static StateSquareMatrix
        computeSampledProcessNoiseCovariance(double dt) {
            StateSquareMatrix Q = StateSquareMatrix::Zero();
            auto dt3 = (dt * dt * dt) / 3;
            auto dt2 = (dt * dt) / 2;
            for (std::size_t xIndex = 0; xIndex < DIMENSION / 2; ++xIndex) {
                auto xDotIndex = xIndex + DIMENSION / 2;
                // xIndex is 'i' and xDotIndex is 'j' in eq. 4.8
                const auto mu = getMu(xDotIndex);
                Q(xIndex, xIndex) = mu * dt3;
                auto symmetric = mu * dt2;
                Q(xIndex, xDotIndex) = symmetric;
                Q(xDotIndex, xIndex) = symmetric;
                Q(xDotIndex, xDotIndex) = mu * dt;
            }
            return Q;
        }

        static double getMu(std::size_t index) {
            BOOST_ASSERT_MSG(index < DIMENSION / 2, "Should only be passing "
                                                    "'i' - the main state, not "
                                                    "the derivative");
            BOOST_ASSERT_MSG(
                false, "Not yet implemented!"); // see figure and table 4.1
            return 1;
        }
        /// Returns a 12-element vector containing a predicted state based on a
        /// constant velocity process model.
        StateVector computeEstimate(State &state, double dt) const {
            StateVector ret = pose_externalized_rotation::applyVelocity(
                state.stateVector(), dt);
            // Dampen velocities
            pose_externalized_rotation::dampenVelocities(ret, m_damp, dt);
            return ret;
        }

      private:
        double m_damp = 0;
    };
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A
