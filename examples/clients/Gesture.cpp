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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/StringIds.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <string>

void gestureCallback(void *userdata, const OSVR_TimeValue * /*timestamp*/,
                     const OSVR_GestureReport *report) {
    auto &ctx = *static_cast<osvr::clientkit::ClientContext *>(userdata);

    /// You would typically not do this every frame - you'd retrieve the ID
    /// based on the string, and then just compare the ID. This is just to make
    /// a more compelling example.
    std::string name = ctx.getGestureNamefromID(osvr::util::StringID(report->gestureID));
    std::cout << "Gesture: Sensor " << report->sensor << ": ID "
              << report->gestureID << " (" << name << ") "
              << (report->state == OSVR_GESTURE_COMPLETE ? "COMPLETE"
                                                         : "IN PROGRESS")
              << std::endl;
}

int main() {

    osvr::clientkit::ClientContext ctx(
        "com.osvr.exampleclients.GestureCallback");

    osvr::clientkit::Interface gesture =
        ctx.getInterface("/com_osvr_example_Gesture/Gesture/gesture");

    gesture.registerCallback(&gestureCallback, &ctx);

    // Pretend that this is your application's mainloop.
    while (1) {
        ctx.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
