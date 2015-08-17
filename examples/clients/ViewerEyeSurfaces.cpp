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
#include <osvr/ClientKit/Display.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    // Start OSVR and get OSVR display config
    osvr::clientkit::ClientContext ctx("com.osvr.example.ViewerEyeSurfaces");
    osvr::clientkit::DisplayConfig display;

    // This is in a loop in case we don't have a display config right away.
    do {
        std::cout << "Trying to get the display config" << std::endl;
        ctx.update();
        display = osvr::clientkit::DisplayConfig(ctx);
    } while (!display.valid());

    // Once we have a valid display config, go through each level with a lambda.
    display.forEachViewer([](osvr::clientkit::Viewer viewer){
        std::cout << "Viewer " << viewer.getViewerID() << "\n";
        viewer.forEachEye([](osvr::clientkit::Eye eye) {
            std::cout << "\tEye " << int(eye.getEyeID()) << "\n";
            eye.forEachSurface([](osvr::clientkit::Surface surface) {
                std::cout << "\t\tSurface " << surface.getSurfaceID() << "\n";
            });
        });
    });

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
