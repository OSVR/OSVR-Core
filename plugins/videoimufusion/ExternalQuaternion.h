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
#include <Eigen/Geometry>

// Standard includes
#include <type_traits>

namespace osvr {
namespace kalman {
    namespace external_quat {
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
        vecToQuat(Eigen::MatrixBase<Derived> const &incRotVec) {
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
            auto epsilon = incRotVec.dot(incRotVec) / 4.;
            Eigen::Quaterniond ret;
            ret.vec() = incRotVec / 2.;
            ret.w() = std::sqrt(1. - epsilon);
            return ret;
        }

        /// Computes what is effectively the Jacobian matrix of partial
        /// derivatives of incrementalOrientationToQuat()
        template <typename Derived>
        inline types::Matrix<4, 3>
        jacobian(Eigen::MatrixBase<Derived> const &incRotVec) {
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
            // eigen internally stores quaternions x, y, z, w
            types::Matrix<4, 3> ret;
            // vector components of jacobian are all 1/2 identity
            ret.topLeftCorner<3, 3>() =
                types::SquareMatrix<3>::Identity() * 0.5;
            ret.bottomRows<1>() =
                incRotVec.transpose() /
                (-4. * sqrt(1. - incRotVec.dot(incRotVec) / 4.));
            return ret;
        }
    } // namespace external_quat
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_ExternalQuaternion_h_GUID_3235DD66_A6E6_47BD_7CC0_51BBF45EF38D
