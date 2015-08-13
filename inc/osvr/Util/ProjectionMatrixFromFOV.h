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

#ifndef INCLUDED_ProjectionMatrixFromFOV_h_GUID_880B5E97_076B_4AB7_2E6E_CC6297F3D85B
#define INCLUDED_ProjectionMatrixFromFOV_h_GUID_880B5E97_076B_4AB7_2E6E_CC6297F3D85B

// Internal Includes
#include <osvr/Util/ProjectionMatrix.h>
#include <osvr/Util/Angles.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Compute a rectangle at unit distance for the given fov values.
    template <typename System>
    inline Rectd computeSymmetricFOVRect(AngleGeneric<System> hFov,
                                         AngleGeneric<System> vFov) {
        // Scale the unit X and Y parameters based on the near
        // plane to make the field of view match what we expect.
        // The tangent of the view angle in either axis is the
        // in-plane distance (left, right, top, or bottom) divided
        // by the distance to the near clipping plane.  We have
        // the angle specified and for now we assume a unit distance
        // to the window (corrected later either in the alternate
        // computeSymmetricFOVRect signature or after caching but before
        // passing to computeProjectionMatrix).  Given this, we solve for
        // the tangent of half the angle (each of left and right provide
        // half, as do top and bottom).
        Rectd ret;
        ret[Rectd::RIGHT] = std::tan(getRadians(hFov / 2.));
        ret[Rectd::LEFT] = -ret[Rectd::RIGHT];
        ret[Rectd::TOP] = std::tan(getRadians(vFov / 2.));
        ret[Rectd::BOTTOM] = -ret[Rectd::TOP];

        return ret;
    }
    /// @brief Compute a rectangle at the near clipping plane for the given
    /// fov values.
    template <typename System>
    inline Rectd computeSymmetricFOVRect(AngleGeneric<System> hFov,
                                         AngleGeneric<System> vFov,
                                         double near) {
        // Get the rectangle at the unit distance
        Rectd ret = computeSymmetricFOVRect(hFov, vFov);

        // Scale the in-plane positions based on the near plane to put
        // the virtual viewing window on the near plane with the eye at the
        // origin.
        ret *= near;
        return ret;
    }

    template <typename System>
    inline Eigen::Matrix4d
    createSymmetricProjectionMatrix(AngleGeneric<System> hFov,
                                    AngleGeneric<System> vFov, double near,
                                    double far) {
        /// Note that because we delegate to the fully-generic projection matrix
        /// code, we'll end up computing two entries in the third column that
        /// will always be zero (and other computations may include an
        /// unnecessary addition/subtraction). But, only one place to introduce
        /// bugs.
        return createProjectionMatrix(computeSymmetricFOVRect(hFov, vFov, near),
                                      near, far);
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_ProjectionMatrixFromFOV_h_GUID_880B5E97_076B_4AB7_2E6E_CC6297F3D85B
