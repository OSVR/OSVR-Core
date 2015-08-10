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
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Client/ViewerEyeSurface.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace client {
    class DisplayConfigFactory;
    class Viewer;
    class ViewerEye {
      public:
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

        OSVR_Pose3 getPose() const;

      private:
        friend class DisplayConfigFactory;
        ViewerEye(Viewer *viewer, OSVR_ClientContext ctx,
                  Eigen::Vector3d const &offset, const char path[]);
        InternalInterfaceOwner m_pose;
        Eigen::Vector3d m_offset;
        std::vector<ViewerEyeSurface> m_surfaces;
        Viewer *m_parent;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_ViewerEye_h_GUID_B895D9B8_5FF1_4991_D174_4AF145F83172
