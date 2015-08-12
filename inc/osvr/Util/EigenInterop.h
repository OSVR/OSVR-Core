/** @file
    @brief Header for interoperation between the Eigen math library, the
   internal mini math library, and VRPN's quatlib

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

#ifndef INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7
#define INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7

// Internal Includes
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/Matrix44C.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenExtras.h>

// Standard includes
// - none

namespace osvr {
namespace util {

    /** @addtogroup UtilMath
    @{
    */

    /// @brief Wrap an OSVR_Vec3 in an Eigen object that allows it to
    /// interoperate with Eigen as though it were an Eigen::Vector3d
    ///
    /// @param vec A vector to wrap
    /// @returns an Eigen::Map allowing use of the OSVR_Vec3 as an
    /// Eigen::Vector3d.
    inline Eigen::Map<Eigen::Vector3d> vecMap(OSVR_Vec3 &vec) {
        return Eigen::Map<Eigen::Vector3d>(&(vec.data[0]));
    }

    /// @overload
    /// For constant vectors.
    inline Eigen::Map<const Eigen::Vector3d> vecMap(OSVR_Vec3 const &vec) {
        return Eigen::Map<const Eigen::Vector3d>(&(vec.data[0]));
    }

    /// @brief Wrap an OSVR_Matrix44 in an Eigen object that allows it to
    /// interoperate with Eigen as though it were an Eigen::Matrix (4x4,
    /// row-major)
    ///
    /// @param mat A matrix to wrap
    /// @returns an Eigen::Map allowing use of the OSVR_Matrix44 as an
    /// Eigen::Matrix.
    inline Eigen::Map<util::RowMatrix44d> matMap(OSVR_Matrix44 &mat) {
        return Eigen::Map<util::RowMatrix44d>(&(mat.data[0][0]));
    }

    /// @overload
    /// For constant matrices.
    inline Eigen::Map<const util::RowMatrix44d>
    matMap(OSVR_Matrix44 const &mat) {
        return Eigen::Map<const util::RowMatrix44d>(&(mat.data[0][0]));
    }

    /// @brief Convert an OSVR_Quaternion to an Eigen::Quaterniond
    inline Eigen::Quaterniond fromQuat(OSVR_Quaternion const &q) {
        return Eigen::Quaterniond(osvrQuatGetW(&q), osvrQuatGetX(&q),
                                  osvrQuatGetY(&q), osvrQuatGetZ(&q));
    }

    /// @brief Convert an Eigen::Quaterniond to a OSVR_Quaternion
    inline void toQuat(Eigen::Quaterniond const &src, OSVR_Quaternion &q) {
        osvrQuatSetW(&q, src.w());
        osvrQuatSetX(&q, src.x());
        osvrQuatSetY(&q, src.y());
        osvrQuatSetZ(&q, src.z());
    }

    /// @brief Turn an OSVR_Pose3 into an Eigen::Transform
    ///
    /// @param pose Input pose
    /// @returns an Eigen::Isometry3d (convertible to an Eigen::Affine3d)
    /// 3-dimensional transform object equivalent to pose.
    inline Eigen::Isometry3d fromPose(OSVR_Pose3 const &pose) {
        Eigen::Isometry3d newPose;
        newPose.fromPositionOrientationScale(vecMap(pose.translation),
                                             fromQuat(pose.rotation),
                                             Eigen::Vector3d::Constant(1));
        return newPose;
    }

    /// @brief Turn an Eigen::Isometry3d (transform) into an OSVR_Pose3
    ///
    /// @param[in] xform Input transform.
    /// @param[out] pose Destination to set based on xform.
    inline void toPose(Eigen::Isometry3d const &xform, OSVR_Pose3 &pose) {
        vecMap(pose.translation) = xform.translation();
        Eigen::Quaterniond quat(xform.rotation());
        toQuat(quat, pose.rotation);
    }

    /// @brief Turn an Eigen::Matrix4d (transform) into an OSVR_Pose3
    ///
    /// @param[in] mat Input transform. Assumed to contain only position and
    /// orientation.
    /// @param[out] pose Destination to set based on xform.
    inline void toPose(Eigen::Matrix4d const &mat, OSVR_Pose3 &pose) {
        Eigen::Affine3d xform(mat);
        vecMap(pose.translation) = xform.translation();
        Eigen::Quaterniond quat(xform.rotation());
        toQuat(quat, pose.rotation);
    }

    /** @} */

} // namespace util
} // namespace osvr
#endif // INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7
