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

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

void myTrackerCallback(void * /*userdata*/,
                       const OSVR_TimeValue * /*timestamp*/,
                       const OSVR_PoseReport *report) {
    std::cout << "Got POSE report: Position = ("
              << report->pose.translation.data[0] << ", "
              << report->pose.translation.data[1] << ", "
              << report->pose.translation.data[2] << "), orientation = ("
              << osvrQuatGetW(&(report->pose.rotation)) << ", ("
              << osvrQuatGetX(&(report->pose.rotation)) << ", "
              << osvrQuatGetY(&(report->pose.rotation)) << ", "
              << osvrQuatGetZ(&(report->pose.rotation)) << "))" << std::endl;
}

void myOrientationCallback(void * /*userdata*/,
                           const OSVR_TimeValue * /*timestamp*/,
                           const OSVR_OrientationReport *report) {
    std::cout << "Got ORIENTATION report: Orientation = ("
              << osvrQuatGetW(&(report->rotation)) << ", ("
              << osvrQuatGetX(&(report->rotation)) << ", "
              << osvrQuatGetY(&(report->rotation)) << ", "
              << osvrQuatGetZ(&(report->rotation)) << "))" << std::endl;
}

void myPositionCallback(void * /*userdata*/,
                        const OSVR_TimeValue * /*timestamp*/,
                        const OSVR_PositionReport *report) {
    std::cout << "Got POSITION report: Position = (" << report->xyz.data[0]
              << ", " << report->xyz.data[1] << ", " << report->xyz.data[2]
              << ")" << std::endl;
}

int main() {
    osvr::clientkit::ClientContext context(
        "com.osvr.exampleclients.TrackerCallback");

    // This is just one of the paths. You can also use:
    // /me/hands/right
    // /me/head
    osvr::clientkit::Interface lefthand =
        context.getInterface("/me/hands/left");

    // The coordinate system is right-handed, with X to the right, Y up, and Z
    // near.
    lefthand.registerCallback(&myTrackerCallback, NULL);

    /// If you just want orientation
    lefthand.registerCallback(&myOrientationCallback, NULL);

    /// or position
    lefthand.registerCallback(&myPositionCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
