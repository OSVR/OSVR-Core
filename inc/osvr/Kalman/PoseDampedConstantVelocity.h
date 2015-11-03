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
#include "PoseConstantVelocity.h"

// Library/third-party includes
// - none

// Standard includes
#include <cassert>

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
        using BaseProcess = PoseConstantVelocityProcessModel;
        using NoiseAutocorrelation = BaseProcess::NoiseAutocorrelation;
        PoseDampedConstantVelocityProcessModel(double damping = 0.1,
                                               double positionNoise = 0.01,
                                               double orientationNoise = 0.1)
            : m_constantVelModel(positionNoise, orientationNoise) {
            setDamping(damping);
        }

        void setNoiseAutocorrelation(double positionNoise = 0.01,
                                     double orientationNoise = 0.1) {
            m_constantVelModel.setNoiseAutocorrelation(positionNoise,
                                                       orientationNoise);
        }

        void setNoiseAutocorrelation(NoiseAutocorrelation const &noise) {
            m_constantVelModel.setNoiseAutocorrelation(noise);
        }
        /// Set the damping - must be positive
        void setDamping(double damping) {
            if (damping > 0) {
                m_damp = damping;
            }
        }

        /// Also known as the "process model jacobian" in TAG, this is A.
        StateSquareMatrix getStateTransitionMatrix(State const &,
                                                   double dt) const {
            return pose_externalized_rotation::
                stateTransitionMatrixWithVelocityDamping(dt, m_damp);
        }

        void predictState(State &s, double dt) {
            auto xHatMinus = computeEstimate(s, dt);
            auto Pminus = predictErrorCovariance(s, *this, dt);
            s.setStateVector(xHatMinus);
            s.setErrorCovariance(Pminus);
        }

        /// This is Q(deltaT) - the Sampled Process Noise Covariance
        /// @return a matrix of dimension n x n. Note that it is real
        /// symmetrical (self-adjoint), so .selfAdjointView<Eigen::Upper>()
        /// might provide useful performance enhancements.
        StateSquareMatrix getSampledProcessNoiseCovariance(double dt) const {
            return m_constantVelModel.getSampledProcessNoiseCovariance(dt);
        }

        /// Returns a 12-element vector containing a predicted state based on a
        /// constant velocity process model.
        StateVector computeEstimate(State &state, double dt) const {
            StateVector ret = m_constantVelModel.computeEstimate(state, dt);
            // Dampen velocities
            pose_externalized_rotation::dampenVelocities(ret, m_damp, dt);
            return ret;
        }

      private:
        BaseProcess m_constantVelModel;
        double m_damp = 0.1;
    };

} // namespace kalman
} // namespace osvr
#endif // INCLUDED_PoseDampedConstantVelocity_h_GUID_FCDCA6AF_D0A2_4D92_49BE_9DBAC5C2F622
