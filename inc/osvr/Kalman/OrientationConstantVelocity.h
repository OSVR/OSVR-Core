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

#ifndef INCLUDED_OrientationConstantVelocity_h_GUID_72B09543_A2CC_458F_2973_7DFD0593F8CC
#define INCLUDED_OrientationConstantVelocity_h_GUID_72B09543_A2CC_458F_2973_7DFD0593F8CC

// Internal Includes
#include "OrientationState.h"

// Library/third-party includes
// - none

// Standard includes
#include <cassert>

namespace osvr {
namespace kalman {
    /// A model for a 3DOF pose (with angular velocity)
    class OrientationConstantVelocityProcessModel {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        using State = orient_externalized_rotation::State;
        using StateVector = orient_externalized_rotation::StateVector;
        using StateSquareMatrix =
            orient_externalized_rotation::StateSquareMatrix;
        using NoiseAutocorrelation = types::Vector<3>;
        OrientationConstantVelocityProcessModel(double orientationNoise = 0.1) {
            setNoiseAutocorrelation(orientationNoise);
        }
        void setNoiseAutocorrelation(double orientationNoise = 0.1) {
            m_mu.head<3>() = types::Vector<3>::Constant(orientationNoise);
        }
        void setNoiseAutocorrelation(NoiseAutocorrelation const &noise) {
            m_mu = noise;
        }

        /// Also known as the "process model jacobian" in TAG, this is A.
        StateSquareMatrix getStateTransitionMatrix(State const &,
                                                   double dt) const {
            return orient_externalized_rotation::stateTransitionMatrix(dt);
        }

        void predictState(State &s, double dt) {
            auto xHatMinus = computeEstimate(s, dt);
            auto Pminus = predictErrorCovariance(s, *this, dt);
            s.setStateVector(xHatMinus);
            s.setErrorCovariance(Pminus);
        }

        /// This is Q(deltaT) - the Sampled Process Noise Covariance
        /// @return a matrix of dimension n x n.
        ///
        /// Like all covariance matrices, it is real symmetrical (self-adjoint),
        /// so .selfAdjointView<Eigen::Upper>() might provide useful performance
        /// enhancements in some algorithms.
        StateSquareMatrix getSampledProcessNoiseCovariance(double dt) const {
            auto const dim = types::Dimension<State>::value;
            StateSquareMatrix cov = StateSquareMatrix::Zero();
            auto dt3 = (dt * dt * dt) / 3;
            auto dt2 = (dt * dt) / 2;
            for (std::size_t xIndex = 0; xIndex < dim / 2; ++xIndex) {
                auto xDotIndex = xIndex + dim / 2;
                // xIndex is 'i' and xDotIndex is 'j' in eq. 4.8
                const auto mu = getMu(xDotIndex);
                cov(xIndex, xIndex) = mu * dt3;
                auto symmetric = mu * dt2;
                cov(xIndex, xDotIndex) = symmetric;
                cov(xDotIndex, xIndex) = symmetric;
                cov(xDotIndex, xDotIndex) = mu * dt;
            }
            return cov;
        }

        /// Returns a 6-element vector containing a predicted state based on a
        /// constant velocity process model.
        StateVector computeEstimate(State &state, double dt) const {
            OSVR_KALMAN_DEBUG_OUTPUT("Time change", dt);
            StateVector ret = orient_externalized_rotation::applyVelocity(
                state.stateVector(), dt);
            return ret;
        }

      private:
        /// this is mu-arrow, the auto-correlation vector of the noise
        /// sources
        NoiseAutocorrelation m_mu;
        double getMu(std::size_t index) const {
            assert(index < types::Dimension<State>::value / 2 &&
                   "Should only be passing "
                   "'i' - the main state, not "
                   "the derivative");
            // This may not be totally correct but it's one of the parameters
            // you can kind of fudge in kalman filters anyway.
            // Should techincally be the diagonal of the correlation kernel of
            // the noise sources. (p77, p197 in Welch 1996)
            return m_mu(index);
        }
    };

} // namespace kalman
} // namespace osvr
#endif // INCLUDED_OrientationConstantVelocity_h_GUID_72B09543_A2CC_458F_2973_7DFD0593F8CC
