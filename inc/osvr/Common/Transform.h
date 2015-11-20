/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Transform_h_GUID_8BF4BBD8_CDC1_48BC_DC27_BFDA42A3212E
#define INCLUDED_Transform_h_GUID_8BF4BBD8_CDC1_48BC_DC27_BFDA42A3212E

// Internal Includes
#include <osvr/Common/Transform_fwd.h>
#include <osvr/Common/DegreesToRadians.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
/// @brief Handles spatial transformations
namespace common {

    /// @brief Spatial transformation, consisting of both pre and post
    /// components.
    class Transform {
      public:
        Transform()
            : m_pre(Eigen::Matrix4d::Identity()),
              m_post(Eigen::Matrix4d::Identity()) {}

        template <typename T1, typename T2>
        Transform(T1 const &pre_matrix, T2 const &post_matrix)
            : m_pre(pre_matrix), m_post(post_matrix) {}

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        template <typename T> void concatPre(T const &xform) { m_pre *= xform; }

        template <typename T> void concatPost(T const &xform) {
            m_post = (xform * m_post).eval();
        }

        /// @brief Update this transformation by the application of another
        /// transformation around it.
        void transform(Transform const &other) {
            concatPre(other.m_pre);
            concatPost(other.m_post);
        }

        /// @brief Apply the transformation to a matrix representing a pose.
        Eigen::Matrix4d transform(Eigen::Matrix4d const &input) const {
            return m_post * input * m_pre;
        }

        /// @brief Apply only the rotation/basis change (not the translation) to
        /// a vector representing a velocity or acceleration
        Eigen::Vector3d
        transformLinear(Eigen::Ref<Eigen::Vector3d const> const &vec) {
            return m_post.topLeftCorner<3, 3>() *
                   (vec.transpose() * m_pre.topLeftCorner<3, 3>()).transpose();
        }

        /// @brief Apply only the rotation/basis change (not the translation) to
        /// a quaternion, typically representing a velocity or acceleration
        Eigen::Quaterniond transformLinear(Eigen::Quaterniond const &quat) {
            ///  @todo figure out the right way to do this. The if-0 section is
            ///  closer but not quite.
            return quat;
#if 0
            Eigen::Quaterniond transformedQuat = Eigen::Quaterniond(
                reorientIsometry(Eigen::Isometry3d(quat)).rotation());
            Eigen::Quaterniond transformedIdentity = Eigen::Quaterniond(
                reorientIsometry(Eigen::Isometry3d::Identity()).rotation());
            return transformedQuat * transformedIdentity.conjugate();
#endif
        }

        Eigen::Matrix4d const &getPre() const { return m_pre; }

        Eigen::Matrix4d const &getPost() const { return m_post; }

      private:
        Eigen::Isometry3d
        reorientIsometry(Eigen::Isometry3d const &input) const {
            Eigen::Isometry3d ret =
                Eigen::Isometry3d(m_post.topLeftCorner<3, 3>()) * input *
                Eigen::Isometry3d(m_pre.topLeftCorner<3, 3>());
            return ret;
        }
        Eigen::Matrix4d m_pre;
        Eigen::Matrix4d m_post;
    };

    template <typename T>
    inline Eigen::Matrix4d rotate(double degrees, T const &axis) {
        return Eigen::Isometry3d(
                   Eigen::AngleAxisd(degreesToRadians(degrees), axis))
            .matrix();
    }

} // namespace common
} // namespace osvr

#endif // INCLUDED_Transform_h_GUID_8BF4BBD8_CDC1_48BC_DC27_BFDA42A3212E
