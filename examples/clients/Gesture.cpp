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

// Library/third-party includes

// Standard includes
#include <iostream>
#include <string>

typedef struct ContextData {
    osvr::clientkit::ClientContext *context;
} ContextData;

void printGestureReport(const OSVR_GestureReport *report, std::string name) {

    std::string state;
    if (report->state == OSVR_GESTURE_COMPLETE) {
        state = "COMPLETE";
    } else {
        state = "IN PROCESS";
    }
    std::cout << name << "; " << report->gestureID << "; " << state << "\t"
              << std::endl;
}

void gestureCallback(void *userdata, const OSVR_TimeValue * /*timestamp*/,
                     const OSVR_GestureReport *report) {
    auto ctx = static_cast<ContextData *>(userdata);
    std::string name = ctx->context->getNamefromID(StringID(report->gestureID));
    std::cout << "Got Gesture Report, for sensor #" << report->sensor
              << std::endl;
    printGestureReport(report, name);
}

int main() {

    ContextData ctx;
    ctx.context = new osvr::clientkit::ClientContext(
        "com.osvr.exampleclients.GestureCallback");

    osvr::clientkit::Interface gesture =
        ctx.context->getInterface("/com_osvr_example_Gesture/Gesture/gesture");

    gesture.registerCallback(&gestureCallback, static_cast<void *>(&ctx));

    // Pretend that this is your application's mainloop.
    while (1) {
        ctx.context->update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}