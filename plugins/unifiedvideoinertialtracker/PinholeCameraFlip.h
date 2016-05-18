/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_PinholeCameraFlip_h_GUID_3FDD8165_096C_4DBA_1E42_20EF78AD28C8
#define INCLUDED_PinholeCameraFlip_h_GUID_3FDD8165_096C_4DBA_1E42_20EF78AD28C8

// Internal Includes
#include "UsefulQuaternions.h"

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
#include <type_traits>

namespace osvr {
namespace vbtracker {
#if 0
    inline void pinholeCameraFlipPose(Eigen::Ref<Eigen::Vector3d> xlate,
                                      Eigen::Quaterniond &rot) {
#endif
    template <typename Derived>
    inline void pinholeCameraFlipPose(Eigen::DenseBase<Derived> &xlate,
                                      Eigen::Quaterniond &rot) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        static_assert(std::is_same<double, typename Derived::Scalar>::value,
                      "Translation scalar type must also be double.");

        /// invert position
        xlate *= -1;
        /// Rotate orientation 180 about Z.
        rot = get180aboutZ() * rot;
    }

#if 0
    inline void
    pinholeCameraFlipVelocities(Eigen::Ref<Eigen::Vector3d> &linVel,
                                Eigen::Ref<Eigen::Vector3d> &angVel) {
#endif
    template <typename Derived1, typename Derived2>
    inline void
    pinholeCameraFlipVelocities(Eigen::DenseBase<Derived1> &linVel,
                                Eigen::DenseBase<Derived2> &angVel) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived1, 3);
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived2, 3);
        static_assert(std::is_same<typename Derived1::Scalar,
                                   typename Derived2::Scalar>::value,
                      "Velocities must have the same scalar type.");

        /// invert velocity
        linVel *= -1;
        /// Rotate angular velocity 180 about Z.
        angVel = get180aboutZ() * angVel;
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_PinholeCameraFlip_h_GUID_3FDD8165_096C_4DBA_1E42_20EF78AD28C8
