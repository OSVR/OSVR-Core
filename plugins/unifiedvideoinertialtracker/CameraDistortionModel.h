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

#ifndef INCLUDED_CameraDistortionModel_h_GUID_9A7F6C37_8AE6_40AC_DB51_C9B7B21F9871
#define INCLUDED_CameraDistortionModel_h_GUID_9A7F6C37_8AE6_40AC_DB51_C9B7B21F9871

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    class CameraDistortionModel {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        CameraDistortionModel(Eigen::Vector2d const &focalLengths,
                              Eigen::Vector2d const &centerOfProjectionPixels,
                              Eigen::Vector3d const &k)
            : m_fl(focalLengths), m_c(centerOfProjectionPixels), m_k(k) {}
        Eigen::Vector2d undistortPoint(Eigen::Vector2d const &pointd) const {
            Eigen::Vector2d normalizedDistorted =
                ((pointd - m_c).array() / m_fl.array()).matrix();
            double r2 = normalizedDistorted.squaredNorm();
            Eigen::Vector2d normalizedUndistorted =
                normalizedDistorted *
                (1 + m_k[0] * r2 + m_k[1] * r2 * r2 + m_k[2] * r2 * r2 * r2);
            Eigen::Vector2d undistorted =
                (normalizedUndistorted.array() * m_fl.array()).matrix() + m_c;
            return undistorted;
        }

      private:
        Eigen::Vector2d m_fl;
        /// assumes center of project is also center of distortion
        Eigen::Vector2d m_c;
        Eigen::Vector3d m_k;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_CameraDistortionModel_h_GUID_9A7F6C37_8AE6_40AC_DB51_C9B7B21F9871
