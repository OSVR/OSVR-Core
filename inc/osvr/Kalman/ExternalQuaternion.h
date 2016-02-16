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
#include <osvr/Util/EigenQuatExponentialMap.h>

// Standard includes
#include <type_traits>
#include <cassert>

namespace osvr {
namespace kalman {
    namespace external_quat {
        /// For use in maintaining an "external quaternion" and 3 incremental
        /// orientations, as done by Welch based on earlier work.
        ///
        /// Performs exponentiation from a vector to a quaternion.
        inline Eigen::Quaterniond vecToQuat(types::Vector<3> const &incRotVec) {
            return util::quat_exp_map(incRotVec).exp();
        }
/// Computes what is effectively the Jacobian matrix of partial
/// derivatives of incrementalOrientationToQuat()
#if 0
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
#endif
#if 0
        inline types::Matrix<4, 3> jacobian(Eigen::Vector3d const &w) {
            double a = w.squaredNorm() / 48 + 0.5;
            // outer product over 24, plus a on the diagonal
            Eigen::Matrix3d topBlock =
                (w * w.transpose()) / 24. + Eigen::Matrix3d::Identity() * a;
            // this weird thing on the bottom row.
            Eigen::RowVector3d bottomRow =
                (Eigen::Vector3d(2 * a, 0, 0) + (w[0] * w) / 12 - w / 4)
                    .transpose();
            types::Matrix<4, 3> ret;
            ret << topBlock, bottomRow;
            return ret;
        }
#endif
    } // namespace external_quat
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_ExternalQuaternion_h_GUID_3235DD66_A6E6_47BD_7CC0_51BBF45EF38D
