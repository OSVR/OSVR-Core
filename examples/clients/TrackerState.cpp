/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

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

// Internal Includes
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/ClientKit/InterfaceStateC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    osvr::clientkit::ClientContext context(
        "com.osvr.exampleclients.TrackerState");

    // This is just one of the paths. You can also use:
    // /me/hands/right
    // /me/head
    osvr::clientkit::Interface lefthand =
        context.getInterface("/me/hands/left");

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        context.update();
        if (i % 100) {
            // Every so often let's read the tracker state.
            // Similar methods exist for all other stock report types.

            // Note that there is not currently a tidy C++ wrapper for
            // state access, so we're using the C API call directly here.
            OSVR_PoseState state;
            OSVR_TimeValue timestamp;
            OSVR_ReturnCode ret =
                osvrGetPoseState(lefthand.get(), &timestamp, &state);
            if (OSVR_RETURN_SUCCESS != ret) {
                std::cout << "No pose state!" << std::endl;
            } else {
                std::cout << "Got POSE state: Position = ("
                          << state.translation.data[0] << ", "
                          << state.translation.data[1] << ", "
                          << state.translation.data[2] << "), orientation = ("
                          << osvrQuatGetW(&(state.rotation)) << ", ("
                          << osvrQuatGetX(&(state.rotation)) << ", "
                          << osvrQuatGetY(&(state.rotation)) << ", "
                          << osvrQuatGetZ(&(state.rotation)) << ")"
                          << std::endl;
            }
        }
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
