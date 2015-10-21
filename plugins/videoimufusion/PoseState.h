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
#include <Eigen/Geometry>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    class PoseWithExternalizedRotation {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const std::size_t DIMENSION = 12;
        using StateVector = Eigen::Matrix<double, DIMENSION, 1>;
        using StateSquareMatrix = Eigen::Matrix<double, DIMENSION, DIMENSION>;
        /// Returns a 12-element vector containing a predicted state based on a
        /// constant velocity process model.
        StateVector computeEstimate(double dt) const {
            // eq. 4.5 in Welch 1996
            auto A = StateSquareMatrix::Identity();
            A.block<3, 3>(0, 3) = Eigen::Matrix3d::Identity() * dt;
            A.block<3, 3>(6, 9) = Eigen::Matrix3d::Identity() * dt;

            return A * m_state;
        }

        StateSquareMatrix
        computeSampledProcessNoiseCovariance(double dt) const {
            auto Q = StateSquareMatrix::Zero();
            for (std::size_t xIndex = 0; xIndex < DIMENSION / 2; ++xIndex) {
                auto xDotIndex = xIndex + DIMENSION / 2;
                // xIndex is 'i' and xDotIndex is 'j' in eq. 4.8
                const auto mu = getMu(xDot);
                Q(xIndex, xIndex) = mu * (dt * dt * dt) / 3 auto symmetric =
                                        mu * (dt * dt) / 2;
                Q(xIndex, xDotIndex) = symmetric;
                Q(xDotIndex, xIndex) = symmetric;
                Q(xDotIndex, xDotIndex) = mu * dt;
            }
            return Q;
        }

        double getMu(std::size_t index) const {
            BOOST_ASSERT_MSG(index < DIMENSION / 2, "Should only be passing "
                                                    "'i' - the main state, not "
                                                    "the derivative");
            BOOST_ASSERT_MSG(
                false, "Not yet implemented!"); // see figure and table 4.1
            return 1;
        }

      private:
        /// In order: x, y, z, incremental rotations phi (about x), theta (about
        /// y), psy (about z), then their derivatives in the same order.
        StateVector m_state;
        /// Externally-maintained orientation per Welch 1996
        Eigen::Quaterniond m_orientation;
    };
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_PoseState_h_GUID_57A246BA_940D_4386_ECA4_4C4172D97F5A
