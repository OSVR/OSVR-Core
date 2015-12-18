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

#ifndef INCLUDED_ExtractYaw_h_GUID_D9365866_476B_429D_7216_17299B9CB5C6
#define INCLUDED_ExtractYaw_h_GUID_D9365866_476B_429D_7216_17299B9CB5C6

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cmath>

namespace osvr {
namespace util {
    /// A utility function to extract "yaw" out of a quaternion, of any
    /// representation as long as it responds to `.w()`, `.x()`, `.y()`, and
    /// `.z()`. Uses atan2() internally, and the return type is automatically
    /// deduced based on the scalar type in use in your quaternion class.
    ///
    /// Reference:
    /// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
    template <typename T>
    inline auto extractYaw(T const &quat) -> decltype(std::atan(quat.w())) {
        auto yaw =
            std::atan2(2 * (quat.y() * quat.w() - quat.x() * quat.z()),
                       1 - 2 * quat.y() * quat.y() - 2 * quat.z() * quat.z());
        return yaw;
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_ExtractYaw_h_GUID_D9365866_476B_429D_7216_17299B9CB5C6
