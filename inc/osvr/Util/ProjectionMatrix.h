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

#ifndef INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107
#define INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107

// Internal Includes
#include <osvr/Util/Rect.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TypePack.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace util {
    /// @brief Takes in points at the near clipping plane, as well as
    /// the near and far clipping planes. Result matrix maps [l, r] and
    /// [b, t] to [-1, 1], and [n, f] to [-1, 1] (should be configurable)
    inline Eigen::Matrix4d createProjectionMatrix(Rectd const &bounds,
                                                  double near, double far) {
        // Convert from "left, right, bottom top, near, far" to the 4x4
        // transform.
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
        // BEGIN APPARENTLY OUTDATED NOTE - works more accurately if you ignore
        // that.
        // NOTE: There is actually a bug in the documentation.  If you
        // call glFrustum() and print out the results and compare them,
        // the value D from that page holds -1 and the value where there
        // is a -1 is what holds D.  This error is also copied to the
        // Microsoft page describing this function.  These are elements
        // [2][3] and [3][2], which are swapped.
        // END APPARENTLY OUTDATED NOTE
        auto right = bounds[Rectd::RIGHT];
        auto left = bounds[Rectd::LEFT];
        auto top = bounds[Rectd::TOP];
        auto bottom = bounds[Rectd::BOTTOM];

        Eigen::Matrix4d mat;
        // clang-format off
        mat << (2 * near / (right - left)), 0, ((right + left) / (right - left)), 0,
               0, (2 * near / (top - bottom)), ((top + bottom) / (top - bottom)), 0,
               0, 0, (-(far + near) / (far - near)), (-2 * far * near / (far - near)),
               0, 0,                           -1,  0;
        // clang-format on
        return mat;
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107
