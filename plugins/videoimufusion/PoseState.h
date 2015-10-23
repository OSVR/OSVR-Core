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
#include <Eigen/Core>
#include <Eigen/Geometry>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace kalman {
    namespace pose_externalized_rotation {
        // static const types::DimensionType DIMENSION = 12;
        using Dimension = types::DimensionConstant<12>;
        using StateVector = types::DimVector<Dimension>;
        using StateVectorBlock3 =
            typename StateVector::template FixedSegmentReturnType<3>::Type;
        using ConstStateVectorBlock3 =
            typename StateVector::template ConstFixedSegmentReturnType<3>::Type;

        using StateVectorBlock6 =
            typename StateVector::template FixedSegmentReturnType<6>::Type;
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
            // eq. 4.5 in Welch 1996
            StateSquareMatrix A = StateSquareMatrix::Identity();
            A.block<3, 3>(0, 3) = Eigen::Matrix3d::Identity() * dt;
            A.block<3, 3>(6, 9) = Eigen::Matrix3d::Identity() * dt;

            return A;
        }
        inline StateSquareMatrix
        stateTransitionMatrixWithVelocityDamping(double dt, double damping) {

            // eq. 4.5 in Welch 1996

            auto A = stateTransitionMatrix(dt);
            auto attenuation = std::pow(damping, dt);
            A.block<6, 6>(6, 6) *= attenuation;
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
        /// Intelligence, IEEE Transactions on, 17(6), 562--575.
        /// http://doi.org/10.1109/34.387503
        template <typename Derived>
        inline typename std::enable_if<Derived::SizeAtCompileTime == 3,
                                       Eigen::Quaterniond>::type
        incrementalOrientationToQuat(
            Eigen::MatrixBase<Derived> const &incRotVec) {
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
            auto epsilon = incRotVec.dot(incRotVec) / 4.;
            Eigen::Quaterniond ret;
            ret.vec() = incRotVec / 2.;
            ret.w() = std::sqrt(1. - epsilon);
            return ret;
        }

        inline Eigen::Quaterniond
        incrementalOrientationToQuat(StateVector const &state) {
            return incrementalOrientationToQuat(incrementalOrientation(state));
        }

        /// Computes what is effectively the Jacobian matrix of partial
        /// derivatives of incrementalOrientationToQuat() (except just taking in
        /// the orientation part)
        template <typename Derived>
        inline types::Matrix<4, 3> incrementalQuaternionJacobian(
            Eigen::MatrixBase<Derived> const &incRotVec) {
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
            // eigen internally stores quaternions x, y, z, w
            types::Matrix<4, 3> ret;
            // vector components of jacobian are all 1/2 identity
            ret.topLeftCorner<3, 3>() =
                types::SquareMatrix<3>::Identity() * 0.5;
            ret.bottomRows<1>() =
                incRotVec.transpose() /
                (-4 * sqrt(1. - incRotVec.dot(incRotVec) / 4.));
            return ret;
        }

        class State {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            static const types::DimensionType DIMENSION = 12;

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
            StateSquareMatrix const &P() const { return m_errorCovariance; }

            void externalizeRotation() {
                m_orientation = getCombinedQuaternion();
                incrementalOrientation(m_state) = Eigen::Vector3d::Zero();
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
    } // namespace pose_externalized_rotation

    /// A basically-constant-velocity model, with the addition of some
    /// damping of the velocities inspired by TAG
    class PoseDampedConstantVelocityProcessModel {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        using State = pose_externalized_rotation::State;
        using StateVector = pose_externalized_rotation::StateVector;
        using StateSquareMatrix = pose_externalized_rotation::StateSquareMatrix;
        static const auto DIMENSION = State::DIMENSION;
        using NoiseAutocorrelation = types::Vector<6>;
        PoseDampedConstantVelocityProcessModel()
            : noiseAutocorrelation(NoiseAutocorrelation::Constant(1)) {}
        /// this is mu-arrow, the auto-correlation vector of the noise
        /// sources
        NoiseAutocorrelation noiseAutocorrelation;

        /// Set the damping - must be non-negative
        void setDamping(double damping) {
            if (damping >= 0) {
                m_damp = damping;
            }
        }
        /// Also known as the "process model jacobian" in TAG, this is A.
        StateSquareMatrix getStateTransitionMatrix(State const &,
                                                   double dt) const {
            return pose_externalized_rotation::
                stateTransitionMatrixWithVelocityDamping(dt, m_damp);
        }

        StateSquareMatrix A(State const &s, double dt) const {
            return getStateTransitionMatrix(s, dt);
        }

        void predictState(State &s, double dt) {
            auto xHatMinus = computeEstimate(s, dt);
            auto Pminus = predictErrorCovariance(s, *this, dt);
            s.setStateVector(xHatMinus);
            s.setErrorCovariance(Pminus);
        }

        /// This is Q(deltaT) - the Sampled Process Noise Covariance
        /// @return a matrix of dimension n x n. Note that it is real
        /// symmetrical (self-adjoint), so .selfAdjointView<Eigen::Upper>() can
        /// provide useful performance enhancements.
        StateSquareMatrix const &Q(double dt) {
            m_cov = StateSquareMatrix::Zero();
            auto dt3 = (dt * dt * dt) / 3;
            auto dt2 = (dt * dt) / 2;
            for (std::size_t xIndex = 0; xIndex < DIMENSION / 2; ++xIndex) {
                auto xDotIndex = xIndex + DIMENSION / 2;
                // xIndex is 'i' and xDotIndex is 'j' in eq. 4.8
                const auto mu = getMu(xDotIndex);
                m_cov(xIndex, xIndex) = mu * dt3;
                auto symmetric = mu * dt2;
                m_cov(xIndex, xDotIndex) = symmetric;
                m_cov(xDotIndex, xIndex) = symmetric;
                m_cov(xDotIndex, xDotIndex) = mu * dt;
            }
            return m_cov;
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
        double getMu(std::size_t index) const {
            BOOST_ASSERT_MSG(index < DIMENSION / 2, "Should only be passing "
                                                    "'i' - the main state, not "
                                                    "the derivative");
            // This may not be totally correct but it's one of the parameters
            // you can kind of fudge in kalman filters anyway.
            // Should techincally be the diagonal of the correlation kernel of
            // the noise sources. (p77, p197 in Welch 1996)
            return noiseAutocorrelation(index);
        }
        StateSquareMatrix m_cov;
        double m_damp = 0;
    };
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A
