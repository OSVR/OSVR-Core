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

void myAnalogCallback(void * /*userdata*/, const OSVR_TimeValue * /*timestamp*/,
                      const OSVR_AnalogReport *report) {
    std::cout << "Got report: channel is " << report->state << std::endl;
}

int main() {
    osvr::clientkit::ClientContext context(
        "com.osvr.exampleclients.AnalogCallback");

    // This is just one of the paths: specifically, the Hydra's left
    // controller's analog trigger. More are in the docs and/or listed on
    // startup
    osvr::clientkit::Interface analogTrigger =
        context.getInterface("/controller/left/trigger");

    analogTrigger.registerCallback(&myAnalogCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        context.update();
    }

    context.log(OSVR_LOGLEVEL_NOTICE, "Library shut down, exiting.");
    return 0;
}
