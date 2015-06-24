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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_GetCameraMatrix_h_GUID_8917F184_7EB3_4680_9E33_B92573F9112D
#define INCLUDED_GetCameraMatrix_h_GUID_8917F184_7EB3_4680_9E33_B92573F9112D

// Internal Includes
// - none

// Library/third-party includes
#include <Eigen/Core>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    typedef Eigen::Matrix3d CameraMatrix;
    /// @brief Create the simple 3x3 camera matrix, given a principal point and
    /// the focal lengths, both in pixels
    inline CameraMatrix
    getCameraMatrix(const Eigen::Ref<const Eigen::Vector2d> &principalPoint,
                    const Eigen::Ref<const Eigen::Vector2d> &focalLengths) {
        CameraMatrix ret;
        // clang-format off
        ret << focalLengths[0], 0, principalPoint[0],
               0, focalLengths[1], principalPoint[1],
               0, 0, 1;
        // clang-format on
    }

    /// @brief Create the simple 3x3 camera matrix, given a principal point and
    /// a focal length (symmetrical between x and y), all in pixels
    inline CameraMatrix
    getCameraMatrix(const Eigen::Ref<const Eigen::Vector2d> &principalPoint,
                    double focalLength) {
        return getCameraMatrix(principalPoint,
                               Eigen::Vector2d::Ones() * focalLength);
    }

    /// @brief Compute a camera matrix given an image width and height in
    /// pixels, "sensor width" in mm, and "focal length" in mm
    inline CameraMatrix getBlenderCameraMatrix(double width, double height,
                                               double mmSensorWidth,
                                               double mmFocalLength) {
        auto focalLength = mmFocalLength * (width / mmSensorWidth);
        return getCameraMatrix(Eigen::Vector2d(width / 2.0, height / 2.0),
                               focalLength);
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_GetCameraMatrix_h_GUID_8917F184_7EB3_4680_9E33_B92573F9112D
