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

#ifndef INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
#define INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C

// Internal Includes
#include <osvr/ClientKit/DisplayC.h>
#include <osvr/ClientKit/Context.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

/// @name Overloads taking output parameters by reference
/// @{
inline OSVR_ReturnCode osvrClientGetNumViewers(OSVR_DisplayConfig disp,
                                               OSVR_ViewerCount &viewers) {
    return osvrClientGetNumViewers(disp, &viewers);
}

inline OSVR_ReturnCode osvrClientGetNumEyesForViewer(OSVR_DisplayConfig disp,
                                                     OSVR_ViewerCount viewer,
                                                     OSVR_EyeCount &eyes) {
    return osvrClientGetNumEyesForViewer(disp, viewer, &eyes);
}

inline OSVR_ReturnCode
osvrClientGetNumSurfacesForViewerEye(OSVR_DisplayConfig disp,
                                     OSVR_ViewerCount viewer, OSVR_EyeCount eye,
                                     OSVR_SurfaceCount &surfaces) {
    return osvrClientGetNumSurfacesForViewerEye(disp, viewer, eye, &surfaces);
}

inline OSVR_ReturnCode osvrClientGetRelativeViewportForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_ViewportDimension &left,
    OSVR_ViewportDimension &bottom, OSVR_ViewportDimension &width,
    OSVR_ViewportDimension &height) {
    return osvrClientGetRelativeViewportForViewerEyeSurface(
        disp, viewer, eye, surface, &left, &bottom, &width, &height);
}

inline OSVR_ReturnCode osvrClientGetProjectionForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, double near, double far, OSVR_Matrix44 &matrix) {
    return osvrClientGetProjectionForViewerEyeSurface(
        disp, viewer, eye, surface, near, far, &matrix);
}
/// @todo ensure we have wrappers for all the functions.
/// @}

namespace osvr {
namespace clientkit {
    typedef shared_ptr<::OSVR_DisplayConfigObject> DisplayConfigPtr;
    inline DisplayConfigPtr getDisplay(ClientContext &ctx) {
        ::OSVR_DisplayConfig cfg;
        osvrClientGetDisplay(ctx.get(), &cfg);
        DisplayConfigPtr ret(cfg, &::osvrClientFreeDisplay);
        return ret;
    }

} // namespace clientkit
} // namespace osvr
#endif // INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
