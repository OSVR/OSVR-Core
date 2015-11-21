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
#include "FlexibleKalmanBase.h"
#include "ExternalQuaternion.h"

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
        using StateVectorBlock3 =
            StateVector::FixedSegmentReturnType<3>::Type;
        using ConstStateVectorBlock3 =
            StateVector::ConstFixedSegmentReturnType<3>::Type;

        using StateVectorBlock6 =
            StateVector::FixedSegmentReturnType<6>::Type;
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
        inline double computeAttenuation(double damping, double dt) {
            return std::pow(damping, dt);
        }
        inline StateSquareMatrix
        stateTransitionMatrixWithVelocityDamping(double dt, double damping) {

            // eq. 4.5 in Welch 1996

            auto A = stateTransitionMatrix(dt);
            auto attenuation = computeAttenuation(damping, dt);
            A.bottomRightCorner<6, 6>() *= attenuation;
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

        inline void dampenVelocities(StateVector &state, double damping,
                                     double dt) {
            auto attenuation = computeAttenuation(damping, dt);
            velocities(state) *= attenuation;
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
                  m_errorCovariance(
                      StateSquareMatrix::
                          Identity() /** @todo almost certainly wrong */),
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

            /// Intended for startup use.
            void setQuaternion(Eigen::Quaterniond const &quaternion) {
                m_orientation = quaternion;
            }

            void postCorrect() { externalizeRotation(); }

            void externalizeRotation() {
                m_orientation = getCombinedQuaternion();
                incrementalOrientation(m_state) = Eigen::Vector3d::Zero();
            }

            void normalizeQuaternion() { m_orientation.normalize(); }

            StateVectorBlock3 getPosition() { return position(m_state); }

            ConstStateVectorBlock3 getPosition() const {
                return position(m_state);
            }

            StateVectorBlock3 getVelocity() { return velocity(m_state); }

            ConstStateVectorBlock3 getVelocity() const {
                return velocity(m_state);
            }

            StateVectorBlock3 getAngularVelocity() {
                return angularVelocity(m_state);
            }

            ConstStateVectorBlock3 getAngularVelocity() const {
                return angularVelocity(m_state);
            }

            Eigen::Quaterniond const &getQuaternion() const {
                return m_orientation;
            }

            Eigen::Quaterniond getCombinedQuaternion() const {
                /// @todo is just quat multiplication OK here? Order right?
                return incrementalOrientationToQuat(m_state) * m_orientation;
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
