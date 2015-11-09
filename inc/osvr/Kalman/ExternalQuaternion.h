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

#ifndef INCLUDED_ExternalQuaternion_h_GUID_3235DD66_A6E6_47BD_7CC0_51BBF45EF38D
#define INCLUDED_ExternalQuaternion_h_GUID_3235DD66_A6E6_47BD_7CC0_51BBF45EF38D

// Internal Includes
#include "FlexibleKalmanBase.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>
#include <cassert>

namespace osvr {
namespace kalman {
    namespace external_quat {

        /// Helper for vecToQuat() and covarianceFromEulerVariance()
        inline double
            vecToQuatScalarPartSquared(types::Vector<3> const &incRotVec) {
            auto epsilon = incRotVec.dot(incRotVec) / 4.;
            return 1. - epsilon;
        }
        /// For use in maintaining an "external quaternion" and 3 incremental
        /// orientations, as done by Welch based on earlier work.
        ///
        /// Can only be used when squared norm of the rotation vector is less
        /// than 1!
        ///
        /// In particular, this function implements equation 6 from a work cited
        /// by Welch,
        /// Azarbayejani, A., & Pentland, A. P. (1995). Recursive estimation of
        /// motion, structure, and focal length. Pattern Analysis and Machine
        /// Intelligence, IEEE Transactions on, 17(6), 562--575.
        /// http://doi.org/10.1109/34.387503
        inline Eigen::Quaterniond vecToQuat(types::Vector<3> const &incRotVec) {
            assert(vecToQuatScalarPartSquared(incRotVec) >= 0 &&
                   "Incremental rotation vector's squared norm was greater "
                   "than 1! Precondition fail!");
            Eigen::Quaterniond ret;
            ret.vec() = incRotVec / 2.;
            ret.w() = std::sqrt(vecToQuatScalarPartSquared(incRotVec));
            return ret;
        }

        /// Computes what is effectively the Jacobian matrix of partial
        /// derivatives of incrementalOrientationToQuat()
        inline types::Matrix<4, 3> jacobian(Eigen::Vector3d const &incRotVec) {
            assert(vecToQuatScalarPartSquared(incRotVec) >= 0 &&
                   "Incremental rotation vector's squared norm was greater "
                   "than 1! Precondition fail!");
            // eigen internally stores quaternions x, y, z, w
            types::Matrix<4, 3> ret;
            // vector components of jacobian are all 1/2 identity
            ret.topLeftCorner<3, 3>() =
                types::SquareMatrix<3>::Identity() * 0.5;
            ret.bottomRows<1>() =
                incRotVec.transpose() /
                (-4. * std::sqrt(vecToQuatScalarPartSquared(incRotVec)));
            return ret;
        }

#if 0
        /// @todo Still seems to be faulty.
        inline types::SquareMatrix<4>
            covarianceFromEulerVariance(types::Vector<3> const &s) {
            auto quatScalarPartSquared = vecToQuatScalarPartSquared(s);
            auto quatScalarPart = std::sqrt(quatScalarPartSquared);

            // bottom-right corner cell
            types::SquareMatrix<4> cov;
            cov(3, 3) = quatScalarPartSquared;

            // Remainder of bottom and right
            types::Vector<3> bottomAndRight = s * quatScalarPart / 2.;
            cov.bottomLeftCorner<1, 3>() = bottomAndRight;
            cov.topRightCorner<3, 1>() = bottomAndRight.transpose();

            // Top-left block
            cov.topLeftCorner<3, 3>() = s * s.transpose() / 4.;

            return cov;
        }
#endif
    } // namespace external_quat
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_ExternalQuaternion_h_GUID_3235DD66_A6E6_47BD_7CC0_51BBF45EF38D
