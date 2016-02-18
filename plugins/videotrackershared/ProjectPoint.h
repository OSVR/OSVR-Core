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

#ifndef INCLUDED_ProjectPoint_h_GUID_11215CA6_05AF_4795_CD97_835F0B623864
#define INCLUDED_ProjectPoint_h_GUID_11215CA6_05AF_4795_CD97_835F0B623864

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// Project point for a simple pinhole camera model, with focal lengths
    /// equal on both axes and no distortion. (Camera intrinsic matrix is
    /// implied - not used directly because it's sparse.)
    ///
    /// This version takes in an point already in camera space.
    inline Eigen::Vector2d projectPoint(double focalLength,
                                        Eigen::Vector2d const &principalPoint,
                                        Eigen::Vector3d const &camPoint) {
        return (camPoint.head<2>() / camPoint[2]) * focalLength +
               principalPoint;
    }

    /// Project point for a simple pinhole camera model, with focal lengths
    /// equal on both axes and no distortion. (Camera intrinsic matrix is
    /// implied - not used directly because it's sparse.)
    ///
    /// This version takes in a point in object space that needs to be
    /// transformed to camera space with a translation and rotation.
    inline Eigen::Vector2d projectPoint(Eigen::Vector3d const &translation,
                                        Eigen::Quaterniond const &rotation,
                                        double focalLength,
                                        Eigen::Vector2d const &principalPoint,
                                        Eigen::Vector3d const &objectPoint) {
        return projectPoint(focalLength, principalPoint,
                            rotation * objectPoint + translation);
    }

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ProjectPoint_h_GUID_11215CA6_05AF_4795_CD97_835F0B623864
