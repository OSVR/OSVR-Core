/** @file
    @brief Header for interoperation between the Eigen math library, the
   internal mini math library, and VRPN's quatlib

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7
#define INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7

// Internal Includes
#include <osvr/Util/Pose3C.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
// - none

namespace osvr {
namespace util {
    inline Eigen::Map<Eigen::Vector3d> vecMap(OSVR_Vec3 &vec) {
        return Eigen::Map<Eigen::Vector3d>(vec.data);
    }

    inline Eigen::Map<const Eigen::Vector3d> vecMap(OSVR_Vec3 const &vec) {
        return Eigen::Map<const Eigen::Vector3d>(vec.data);
    }

    inline Eigen::Isometry3d fromPose(OSVR_Pose3 const &pose) {
        Eigen::Map<const Eigen::Vector3d> xlateVec(pose.translation.data);
        return Eigen::Translation3d(vecMap(pose.translation)) *
               Eigen::Quaterniond(pose.rotation.data);
    }

    inline void toPose(Eigen::Isometry3d const &xform, OSVR_Pose3 &pose) {
        Eigen::Vector3d::Map(pose.translation.data) = xform.translation();
        Eigen::Quaterniond quat(xform.rotation());
        Eigen::Vector3d::Map(&pose.rotation.data[1]) = quat.vec();
        pose.rotation.data[0] = quat.w();
    }

} // namespace util
} // namespace osvr
#endif // INCLUDED_EigenInterop_h_GUID_A07DFDC3_BA71_4894_6593_732B364A2AE7
