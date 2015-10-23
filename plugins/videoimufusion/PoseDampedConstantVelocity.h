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

#ifndef INCLUDED_PoseDampedConstantVelocity_h_GUID_FCDCA6AF_D0A2_4D92_49BE_9DBAC5C2F622
#define INCLUDED_PoseDampedConstantVelocity_h_GUID_FCDCA6AF_D0A2_4D92_49BE_9DBAC5C2F622

// Internal Includes
#include "PoseState.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace kalman {
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
#endif // INCLUDED_PoseDampedConstantVelocity_h_GUID_FCDCA6AF_D0A2_4D92_49BE_9DBAC5C2F622
