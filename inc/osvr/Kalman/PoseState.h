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
#include "ExternalQuaternion.h"
#include "FlexibleKalmanBase.h"

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
// - none

namespace osvr {
namespace kalman {
    namespace pose_externalized_rotation {
        using Dimension = types::DimensionConstant<12>;
        using StateVector = types::DimVector<Dimension>;
        using StateVectorBlock3 = StateVector::FixedSegmentReturnType<3>::Type;
        using ConstStateVectorBlock3 =
            StateVector::ConstFixedSegmentReturnType<3>::Type;

        using StateVectorBlock6 = StateVector::FixedSegmentReturnType<6>::Type;
        using StateSquareMatrix = types::DimSquareMatrix<Dimension>;

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

        /// This returns A(deltaT), though if you're just predicting xhat-, use
        /// applyVelocity() instead for performance.
        inline StateSquareMatrix stateTransitionMatrix(double dt) {
            // eq. 4.5 in Welch 1996 - except we have all the velocities at the
            // end
            StateSquareMatrix A = StateSquareMatrix::Identity();
            A.topRightCorner<6, 6>() = types::SquareMatrix<6>::Identity() * dt;

            return A;
        }
        /// Function used to compute the coefficient m in v_new = m * v_old.
        /// The damping value is for exponential decay.
        inline double computeAttenuation(double damping, double dt) {
            return std::pow(damping, dt);
        }

        /// Returns the state transition matrix for a constant velocity with a
        /// single damping parameter (not for direct use in computing state
        /// transition, because it is very sparse, but in computing other
        /// values)
        inline StateSquareMatrix
        stateTransitionMatrixWithVelocityDamping(double dt, double damping) {
            // eq. 4.5 in Welch 1996
            auto A = stateTransitionMatrix(dt);
            A.bottomRightCorner<6, 6>() *= computeAttenuation(damping, dt);
            return A;
        }

        /// Returns the state transition matrix for a constant velocity with
        /// separate damping paramters for linear and angular velocity (not for
        /// direct use in computing state transition, because it is very sparse,
        /// but in computing other values)
        inline StateSquareMatrix
        stateTransitionMatrixWithSeparateVelocityDamping(double dt,
                                                         double posDamping,
                                                         double oriDamping) {
            // eq. 4.5 in Welch 1996
            auto A = stateTransitionMatrix(dt);
            A.block<3, 3>(6, 6) *= computeAttenuation(posDamping, dt);
            A.bottomRightCorner<3, 3>() *= computeAttenuation(oriDamping, dt);
            return A;
        }

        /// Computes A(deltaT)xhat(t-deltaT)
        inline StateVector applyVelocity(StateVector const &state, double dt) {
            // eq. 4.5 in Welch 1996

            /// @todo benchmark - assuming for now that the manual small
            /// calcuations are faster than the matrix ones.

            StateVector ret = state;
            position(ret) += velocity(state) * dt;
            incrementalOrientation(ret) += angularVelocity(state) * dt;
            return ret;
        }

        /// Dampen all 6 components of velocity by a single factor.
        inline void dampenVelocities(StateVector &state, double damping,
                                     double dt) {
            auto attenuation = computeAttenuation(damping, dt);
            velocities(state) *= attenuation;
        }

        /// Separately dampen the linear and angular velocities
        inline void separatelyDampenVelocities(StateVector &state,
                                               double posDamping,
                                               double oriDamping, double dt) {
            velocity(state) *= computeAttenuation(posDamping, dt);
            angularVelocity(state) *= computeAttenuation(oriDamping, dt);
        }

        inline Eigen::Quaterniond
        incrementalOrientationToQuat(StateVector const &state) {
            return external_quat::vecToQuat(incrementalOrientation(state));
        }

        class State : public HasDimension<12> {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW

            /// Default constructor
            State()
                : m_state(StateVector::Zero()),
                  m_errorCovariance(StateSquareMatrix::Identity() *
                                    10 /** @todo almost certainly wrong */),
                  m_orientation(Eigen::Quaterniond::Identity()) {}
            /// set xhat
            void setStateVector(StateVector const &state) { m_state = state; }
            /// xhat
            StateVector const &stateVector() const { return m_state; }
            // set P
            void setErrorCovariance(StateSquareMatrix const &errorCovariance) {
                m_errorCovariance = errorCovariance;
            }
            /// P
            StateSquareMatrix const &errorCovariance() const {
                return m_errorCovariance;
            }
            StateSquareMatrix &errorCovariance() { return m_errorCovariance; }

            /// Intended for startup use.
            void setQuaternion(Eigen::Quaterniond const &quaternion) {
                m_orientation = quaternion.normalized();
            }

            void postCorrect() { externalizeRotation(); }

            void externalizeRotation() {
                setQuaternion(getCombinedQuaternion());
                incrementalOrientation() = Eigen::Vector3d::Zero();
            }

            StateVectorBlock3 position() {
                return pose_externalized_rotation::position(m_state);
            }

            ConstStateVectorBlock3 position() const {
                return pose_externalized_rotation::position(m_state);
            }

            StateVectorBlock3 incrementalOrientation() {
                return pose_externalized_rotation::incrementalOrientation(
                    m_state);
            }

            ConstStateVectorBlock3 incrementalOrientation() const {
                return pose_externalized_rotation::incrementalOrientation(
                    m_state);
            }

            StateVectorBlock3 velocity() {
                return pose_externalized_rotation::velocity(m_state);
            }

            ConstStateVectorBlock3 velocity() const {
                return pose_externalized_rotation::velocity(m_state);
            }

            StateVectorBlock3 angularVelocity() {
                return pose_externalized_rotation::angularVelocity(m_state);
            }

            ConstStateVectorBlock3 angularVelocity() const {
                return pose_externalized_rotation::angularVelocity(m_state);
            }

            Eigen::Quaterniond const &getQuaternion() const {
                return m_orientation;
            }

            Eigen::Quaterniond getCombinedQuaternion() const {
                /// @todo is just quat multiplication OK here? Order right?
                return incrementalOrientationToQuat(m_state).normalized() *
                       m_orientation;
            }

            /// Get the position and quaternion combined into a single isometry
            /// (transformation)
            Eigen::Isometry3d getIsometry() const {
                Eigen::Isometry3d ret;
                ret.fromPositionOrientationScale(position(), getQuaternion(),
                                                 Eigen::Vector3d::Constant(1));
                return ret;
            }

          private:
            /// In order: x, y, z, incremental rotations phi (about x), theta
            /// (about y), psy (about z), then their derivatives in the same
            /// order.
            StateVector m_state;
            /// P
            StateSquareMatrix m_errorCovariance;
            /// Externally-maintained orientation per Welch 1996
            Eigen::Quaterniond m_orientation;
        };

        /// Stream insertion operator, for displaying the state of the state
        /// class.
        template <typename OutputStream>
        inline OutputStream &operator<<(OutputStream &os, State const &state) {
            os << "State:" << state.stateVector().transpose() << "\n";
            os << "quat:" << state.getCombinedQuaternion().coeffs().transpose()
               << "\n";
            os << "error:\n" << state.errorCovariance() << "\n";
            return os;
        }
    } // namespace pose_externalized_rotation
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A
