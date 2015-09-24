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

#ifndef INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30
#define INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Client/DisplayInput.h>
#include <osvr/Client/Viewer.h>
#include <osvr/Client/Viewers.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Util/ContainerWrapper.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace client {

    class DisplayConfig;
    typedef unique_ptr<DisplayConfig> DisplayConfigPtr;
    class DisplayConfigFactory {
      public:
        OSVR_CLIENT_EXPORT static DisplayConfigPtr
        create(OSVR_ClientContext ctx);
    };

    class DisplayConfig {
      public:
        DisplayConfig(DisplayConfig const &) = delete;
        DisplayConfig &operator=(DisplayConfig const &) = delete;
        OSVR_ViewerCount getNumViewers() const;
        Viewer &getViewer(OSVR_ViewerCount viewer);
        Viewer const &getViewer(OSVR_ViewerCount viewer) const;

        OSVR_EyeCount getNumViewerEyes(OSVR_ViewerCount viewer) const;
        ViewerEye &getViewerEye(OSVR_ViewerCount viewer, OSVR_EyeCount eye);
        ViewerEye const &getViewerEye(OSVR_ViewerCount viewer,
                                      OSVR_EyeCount eye) const;

        OSVR_SurfaceCount getNumViewerEyeSurfaces(OSVR_ViewerCount viewer,
                                                  OSVR_EyeCount eye) const;
        ViewerEye &getViewerEyeSurface(OSVR_ViewerCount viewer,
                                       OSVR_EyeCount eye,
                                       OSVR_SurfaceCount surface);
        ViewerEye const &getViewerEyeSurface(OSVR_ViewerCount viewer,
                                             OSVR_EyeCount eye,
                                             OSVR_SurfaceCount surface) const;
        OSVR_DisplayInputCount getNumDisplayInputs() const;
        DisplayInput &getDisplayInput(OSVR_DisplayInputCount dispInputIdx);
        const DisplayInput &
        getDisplayInput(OSVR_DisplayInputCount dispInputIdx) const;

        OSVR_CLIENT_EXPORT bool isStartupComplete() const;

      private:
        std::vector<DisplayInput> m_displayInputs;
        Viewers m_viewers;
        friend class DisplayConfigFactory;
        DisplayConfig();
    };

    //-- inline implementations --//
    inline OSVR_ViewerCount DisplayConfig::getNumViewers() const {
        return static_cast<OSVR_ViewerCount>(m_viewers.size());
    }

    inline Viewer &DisplayConfig::getViewer(OSVR_ViewerCount viewer) {
        return m_viewers[viewer];
    }
    inline Viewer const &
    DisplayConfig::getViewer(OSVR_ViewerCount viewer) const {
        return m_viewers[viewer];
    }

    inline OSVR_EyeCount
    DisplayConfig::getNumViewerEyes(OSVR_ViewerCount viewer) const {
        return getViewer(viewer).size();
    }

    inline ViewerEye &DisplayConfig::getViewerEye(OSVR_ViewerCount viewer,
                                                  OSVR_EyeCount eye) {
        return getViewer(viewer)[eye];
    }
    inline ViewerEye const &
    DisplayConfig::getViewerEye(OSVR_ViewerCount viewer,
                                OSVR_EyeCount eye) const {
        return getViewer(viewer)[eye];
    }
    inline OSVR_SurfaceCount
    DisplayConfig::getNumViewerEyeSurfaces(OSVR_ViewerCount viewer,
                                           OSVR_EyeCount eye) const {
        return getViewerEye(viewer, eye).size();
    }
    inline ViewerEye &
    DisplayConfig::getViewerEyeSurface(OSVR_ViewerCount viewer,
                                       OSVR_EyeCount eye, OSVR_SurfaceCount) {
        /// @todo right now only a single surface per viewer eye
        return getViewerEye(viewer, eye);
    }
    inline ViewerEye const &DisplayConfig::getViewerEyeSurface(
        OSVR_ViewerCount viewer, OSVR_EyeCount eye, OSVR_SurfaceCount) const {
        /// @todo right now only a single surface per viewer eye
        return getViewerEye(viewer, eye);
    }
    inline OSVR_DisplayInputCount DisplayConfig::getNumDisplayInputs() const {
        return static_cast<OSVR_DisplayInputCount>(m_displayInputs.size());
    }
    inline DisplayInput &
    DisplayConfig::getDisplayInput(OSVR_DisplayInputCount dispInputIdx) {
        return m_displayInputs[dispInputIdx];
    }
    inline DisplayInput const &
    DisplayConfig::getDisplayInput(OSVR_DisplayInputCount dispInputIdx) const {
        return m_displayInputs[dispInputIdx];
    }

} // namespace client
} // namespace osvr

#endif // INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30
