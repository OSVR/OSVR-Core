/** @file
    @brief Header

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

#ifndef INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B
#define INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/EigenInterop.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
// - none

namespace osvr {
namespace client {
    class NullTrackerTransform {
      public:
        void operator()(OSVR_PoseReport &) {}
    };

    class HydraTrackerTransform {
      public:
        void operator()(OSVR_PoseReport &report) {
            Eigen::Isometry3d pose = util::fromPose(report.pose);
            // Rotate -90 about X
            util::toPose(
                Eigen::AngleAxisd(-0.5 * M_PI, Eigen::Vector3d::UnitX()) * pose,
                report.pose);
        }
    };

} // namespace client
} // namespace osvr
#endif // INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B
