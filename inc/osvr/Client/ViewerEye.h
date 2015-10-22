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

#ifndef INCLUDED_ViewerEye_h_GUID_B895D9B8_5FF1_4991_D174_4AF145F83172
#define INCLUDED_ViewerEye_h_GUID_B895D9B8_5FF1_4991_D174_4AF145F83172

// Internal Includes
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/RenderingTypesC.h>
#include <osvr/Client/Export.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/Rect.h>
#include <osvr/Util/MatrixConventionsC.h>
#include <osvr/Util/RadialDistortionParametersC.h>
#include <osvr/Util/Angles.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <vector>
#include <stdexcept>
#include <utility>

namespace osvr {
namespace client {
    class DisplayConfigFactory;
    struct Viewport {
        int32_t left;
        int32_t bottom;
        int32_t width;
        int32_t height;
    };
    struct NoPoseYet : std::runtime_error {
        NoPoseYet()
            : std::runtime_error("No pose data yet for the interface!") {}
    };
    class ViewerEye {
      public:
        ViewerEye(ViewerEye const &) = delete;
        ViewerEye &operator=(ViewerEye const &) = delete;
        ViewerEye(ViewerEye &&other)
            : m_pose(std::move(other.m_pose)), m_offset(other.m_offset),
              m_viewport(other.m_viewport),
              m_unitBounds(std::move(other.m_unitBounds)),
              m_rot180(other.m_rot180), m_pitchTilt(other.m_pitchTilt),
              m_radDistortParams(std::move(other.m_radDistortParams)),
              m_displayInputIdx(other.m_displayInputIdx),
              m_opticalAxisOffsetY(other.m_opticalAxisOffsetY) {}

        inline OSVR_SurfaceCount size() const { return 1; }
#if 0
        inline OSVR_SurfaceCount size() const {
            return static_cast<OSVR_SurfaceCount>(m_surfaces.size());
        }
        inline ViewerEyeSurface &operator[](OSVR_SurfaceCount index) {
            return m_surfaces[index];
        }
        inline ViewerEyeSurface const &
        operator[](OSVR_SurfaceCount index) const {
            return m_surfaces[index];
        }
#endif
        OSVR_CLIENT_EXPORT OSVR_Pose3 getPose() const;
        OSVR_CLIENT_EXPORT bool hasPose() const;

        OSVR_CLIENT_EXPORT Eigen::Matrix4d getView() const;

        bool wantDistortion() const {
            return m_radDistortParams.is_initialized();
        }

        boost::optional<OSVR_RadialDistortionParameters>
        getRadialDistortionParams() const {
            return m_radDistortParams;
        }

        OSVR_DistortionPriority getRadialDistortionPriority() const {
            return (m_radDistortParams.is_initialized())
                       ? 1
                       : OSVR_DISTORTION_PRIORITY_UNAVAILABLE;
        }

        /// @brief Gets a matrix that takes in row vectors in a right-handed
        /// system and outputs signed Z.
        OSVR_CLIENT_EXPORT Eigen::Matrix4d getProjection(double near,
                                                         double far) const;
        OSVR_CLIENT_EXPORT Eigen::Matrix4d
        getProjection(double near, double far,
                      OSVR_MatrixConventions flags) const;

        /// @brief Gets clipping planes for a given surface
        OSVR_CLIENT_EXPORT util::Rectd getRect() const;

        Viewport getDisplayRelativeViewport() const { return m_viewport; }

        OSVR_DisplayInputCount getDisplayInputIdx() const {
            return m_displayInputIdx;
        }

      private:
        friend class DisplayConfigFactory;
        ViewerEye(
            OSVR_ClientContext ctx, Eigen::Vector3d const &offset,
            const char path[], Viewport &&viewport, util::Rectd &&unitBounds,
            bool rot180, double pitchTilt,
            boost::optional<OSVR_RadialDistortionParameters> radDistortParams,
            OSVR_DisplayInputCount displayInputIdx,
            util::Angle opticalAxisOffsetY = 0. * util::radians);
        util::Rectd m_getRect(double near, double far) const;
        Eigen::Isometry3d getPoseIsometry() const;
        InternalInterfaceOwner m_pose;
        Eigen::Vector3d m_offset;
#if 0
        std::vector<ViewerEyeSurface> m_surfaces;
#endif
        Viewport m_viewport;
        util::Rectd m_unitBounds;

        bool m_rot180;
        double m_pitchTilt;
        boost::optional<OSVR_RadialDistortionParameters> m_radDistortParams;
        OSVR_DisplayInputCount m_displayInputIdx;
        util::Angle m_opticalAxisOffsetY;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_ViewerEye_h_GUID_B895D9B8_5FF1_4991_D174_4AF145F83172
