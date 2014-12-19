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
    class NullTransform {
      public:
        template <typename T> void operator()(T &) {}
    };

    /// The VRPN driver for some devices uses the non-native but relatively
    /// de-facto VRPN standard of right-handed x-right y-far z-up.
    class ZUpTrackerTransform {
      public:
        void operator()(OSVR_PoseReport &report) {
            Eigen::Matrix4d changeOfCS(Eigen::Matrix4d::Zero());
            changeOfCS(0, 0) = 1;  // X stays the same
            changeOfCS(1, 2) = 1;  // Z becomes Y
            changeOfCS(2, 1) = -1; // -Y becomes Z
            changeOfCS(3, 3) = 1;  // homogeneous
            Eigen::Matrix4d pose =
                changeOfCS * util::fromPose(report.pose).matrix() *
                Eigen::Isometry3d(
                    Eigen::AngleAxisd(0.5 * M_PI,
                                      Eigen::Vector3d::UnitX()) /// postrotate
                                                                /// to adjust
                                                                /// the sensors
                    ).matrix();

            util::toPose(pose, report.pose);
        }
    };

    class CustomPostrotateTransform {
      public:
        CustomPostrotateTransform(double angle, Eigen::Vector3d const &axis)
            : m_angle(angle), m_axis(axis) {}
        void operator()(OSVR_PoseReport &report) {
            Eigen::Matrix4d pose =
                util::fromPose(report.pose).matrix() *
                Eigen::Isometry3d(Eigen::AngleAxisd(m_angle, m_axis)).matrix();

            util::toPose(pose, report.pose);
        }

      private:
        double m_angle;
        Eigen::Vector3d m_axis;
    };

    template <typename T1, typename T2> class CombinedTransforms {
      public:
        CombinedTransforms(T1 const &a, T2 const &b) : m_xforms(a, b) {}
        template <typename T> void operator()(T &report) {
            m_xforms.first(report);
            m_xforms.second(report);
        }

      private:
        std::pair<T1, T2> m_xforms;
    };

    template <typename T1, typename T2>
    inline CombinedTransforms<T1, T2> combineTransforms(T1 const &a,
                                                        T2 const &b) {
        return CombinedTransforms<T1, T2>(a, b);
    }

} // namespace client
} // namespace osvr
#endif // INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B
