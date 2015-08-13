/** @file
    @brief Implementation

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/DisplayC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    // Start OSVR and get OSVR display config
    osvr::clientkit::ClientContext ctx("com.osvr.example.ViewerEyeSurfaces");
    OSVR_DisplayConfig display;

    // This is in a loop in case we don't have a display config right away.
    OSVR_ReturnCode ret = OSVR_RETURN_FAILURE;
    do {
        std::cout << "Trying to get the display config" << std::endl;
        ret = osvrClientGetDisplay(ctx.get(), &display);
    } while (ret == OSVR_RETURN_FAILURE);

    OSVR_ViewerCount viewers;
    osvrClientGetNumViewers(display, &viewers);
    for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {
        std::cout << "Viewer " << viewer << "\n";
        OSVR_EyeCount eyes;
        osvrClientGetNumEyesForViewer(display, viewer, &eyes);

        for (OSVR_EyeCount eye = 0; eye < eyes; ++eye) {
            std::cout << "\tEye " << int(eye) << "\n";
            OSVR_SurfaceCount surfaces;
            osvrClientGetNumSurfacesForViewerEye(display, viewer, eye,
                                                 &surfaces);
            for (OSVR_SurfaceCount surface = 0; surface < surfaces; ++surface) {
                std::cout << "\t\tSurface " << surface << "\n";
            }
        }
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
