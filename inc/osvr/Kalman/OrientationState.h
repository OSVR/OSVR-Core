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

#ifndef INCLUDED_OrientationState_h_GUID_B2EA5856_0B18_43B1_CE18_8B7385E607CA
#define INCLUDED_OrientationState_h_GUID_B2EA5856_0B18_43B1_CE18_8B7385E607CA

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
    namespace orient_externalized_rotation {
        using Dimension = types::DimensionConstant<6>;
        using StateVector = types::DimVector<Dimension>;
        using StateVectorBlock3 = StateVector::FixedSegmentReturnType<3>::Type;
        using ConstStateVectorBlock3 =
            StateVector::ConstFixedSegmentReturnType<3>::Type;

        using StateSquareMatrix = types::DimSquareMatrix<Dimension>;

        /// @name Accessors to blocks in the state vector.
        /// @{
        inline StateVectorBlock3 incrementalOrientation(StateVector &vec) {
            return vec.head<3>();
        }
        inline ConstStateVectorBlock3
        incrementalOrientation(StateVector const &vec) {
            return vec.head<3>();
        }

        inline StateVectorBlock3 angularVelocity(StateVector &vec) {
            return vec.tail<3>();
        }
        inline ConstStateVectorBlock3 angularVelocity(StateVector const &vec) {
            return vec.tail<3>();
        }
        /// @}

        /// This returns A(deltaT), though if you're just predicting xhat-, use
        /// applyVelocity() instead for performance.
        inline StateSquareMatrix stateTransitionMatrix(double dt) {
            StateSquareMatrix A = StateSquareMatrix::Identity();
            A.topRightCorner<3, 3>() = types::SquareMatrix<3>::Identity() * dt;
            return A;
        }
        inline StateSquareMatrix
        stateTransitionMatrixWithVelocityDamping(double dt, double damping) {

            // eq. 4.5 in Welch 1996

            auto A = stateTransitionMatrix(dt);
            auto attenuation = std::pow(damping, dt);
            A.bottomRightCorner<3, 3>() *= attenuation;
            return A;
        }
        /// Computes A(deltaT)xhat(t-deltaT)
        inline StateVector applyVelocity(StateVector const &state, double dt) {
            // eq. 4.5 in Welch 1996

            /// @todo benchmark - assuming for now that the manual small
            /// calcuations are faster than the matrix ones.

            StateVector ret = state;
            incrementalOrientation(ret) += angularVelocity(state) * dt;
            return ret;
        }

        inline void dampenVelocities(StateVector &state, double damping,
                                     double dt) {
            auto attenuation = std::pow(damping, dt);
            angularVelocity(state) *= attenuation;
        }

        inline Eigen::Quaterniond
        incrementalOrientationToQuat(StateVector const &state) {
            return external_quat::vecToQuat(incrementalOrientation(state));
        }

        class State : public HasDimension<6> {
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
                m_orientation = quaternion.normalized();
            }

            void postCorrect() { externalizeRotation(); }

            void externalizeRotation() {
                m_orientation = getCombinedQuaternion();
                incrementalOrientation(m_state) = Eigen::Vector3d::Zero();
            }

            void normalizeQuaternion() { m_orientation.normalize(); }

            StateVectorBlock3 angularVelocity() {
                return orient_externalized_rotation::angularVelocity(m_state);
            }

            ConstStateVectorBlock3 angularVelocity() const {
                return orient_externalized_rotation::angularVelocity(m_state);
            }

            Eigen::Quaterniond const &getQuaternion() const {
                return m_orientation;
            }

            Eigen::Quaterniond getCombinedQuaternion() const {
                /// @todo is just quat multiplication OK here? Order right?
                return (incrementalOrientationToQuat(m_state) * m_orientation)
                    .normalized();
            }

          private:
            /// In order: x, y, z, orientation , then its derivatives in the
            /// same
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
    } // namespace orient_externalized_rotation
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_OrientationState_h_GUID_B2EA5856_0B18_43B1_CE18_8B7385E607CA
