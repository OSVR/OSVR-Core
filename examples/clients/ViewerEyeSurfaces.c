/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2014 Sensics, Inc.
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
*/

/* Internal Includes */
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/DisplayC.h>

/* Library/third-party includes */
/* - none */

// Standard includes
#include <stdio.h>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.example.ViewerEyeSurfaces", 0);
    OSVR_DisplayConfig display;

    /* This is in a loop in case we don't have a display config right away. */
    OSVR_ReturnCode ret = OSVR_RETURN_FAILURE;
    do {
        printf("Trying to get the display config\n");
        osvrClientUpdate(ctx);
        ret = osvrClientGetDisplay(ctx, &display);
    } while (ret == OSVR_RETURN_FAILURE);

    OSVR_ViewerCount viewers;
    osvrClientGetNumViewers(display, &viewers);

    OSVR_ViewerCount viewer;
    for (viewer = 0; viewer < viewers; ++viewer) {
        printf("Viewer %d\n", viewer);
        OSVR_EyeCount eyes;
        osvrClientGetNumEyesForViewer(display, viewer, &eyes);

        OSVR_EyeCount eye;
        for (eye = 0; eye < eyes; ++eye) {
            printf("\tEye %d\n", eye);
            OSVR_SurfaceCount surfaces;
            osvrClientGetNumSurfacesForViewerEye(display, viewer, eye,
                                                 &surfaces);

            OSVR_SurfaceCount surface;
            for (surface = 0; surface < surfaces; ++surface) {
                printf("\t\tSurface %d\n", surface);
            }
        }
    }
    osvrClientFreeDisplay(display);
    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
    return 0;
}
