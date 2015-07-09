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
#include <osvr/ClientKit/Interface.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes

// Standard includes
#include <iostream>
#include <string>

void printEyeTracker2DReport(const OSVR_EyeTracker2DReport *report) {

    std::cout << "2D Data:" << report->state.data[0] << "; "
              << report->state.data[1] << std::endl;
}

void printEyeTracker3DReport(const OSVR_EyeTracker3DReport *report) {

    std::cout << "3D Data Base Point:" << report->state.basePoint.data[0]
              << "; " << report->state.basePoint.data[1] << "; "
              << report->state.basePoint.data[2] << std::endl;
    std::cout << "3D Data Direction:" << report->state.direction.data[0] << "; "
              << report->state.direction.data[1] << "; "
              << report->state.direction.data[2] << std::endl;
}

void printEyeTrackerBlinkReport(const OSVR_EyeTrackerBlinkReport *report) {

    if (report->state == OSVR_EYE_BLINK) {
        std::cout << "Blink occured" << std::endl;
    } else {
        std::cout << "No blink" << std::endl;
    }
}

void eyeTracker2DCallback(void * /*userdata*/,
                          const OSVR_TimeValue * /*timestamp*/,
                          const OSVR_EyeTracker2DReport *report) {
    std::cout << "Got 2D Eye Tracker Report: for sensor #" << report->sensor
              << std::endl;
    printEyeTracker2DReport(report);
}

void eyeTracker3DCallback(void * /*userdata*/,
                          const OSVR_TimeValue * /*timestamp*/,
                          const OSVR_EyeTracker3DReport *report) {
    std::cout << "Got 3D Eye Tracker Report: for sensor #" << report->sensor
              << std::endl;
    printEyeTracker3DReport(report);
}

void eyeTrackerBlinkCallback(void * /*userdata*/,
                             const OSVR_TimeValue * /*timestamp*/,
                             const OSVR_EyeTrackerBlinkReport *report) {
    std::cout << "Got Eye Tracker Blink Report: for sensor #" << report->sensor
              << std::endl;
    printEyeTrackerBlinkReport(report);
}

int main() {
    osvr::clientkit::ClientContext context(
        "com.osvr.exampleclients.EyeTrackerCallback");

    osvr::clientkit::Interface eyetracker = context.getInterface(
        "/com_osvr_example_EyeTracker/EyeTracker/eyetracker");

    eyetracker.registerCallback(&eyeTracker2DCallback, NULL);
    eyetracker.registerCallback(&eyeTracker3DCallback, NULL);
    eyetracker.registerCallback(&eyeTrackerBlinkCallback, NULL);

    // Pretend that this is your application's mainloop.
    while (1) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
