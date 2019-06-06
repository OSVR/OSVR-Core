/** @file
    @brief Test Implementation

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
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/JointClientKit/JointClientKitC.h>

// Library/third-party includes
// - none

// Standard includes
#include <catch2/catch.hpp>

static bool reportReceived = false;

static void myAnalogCallback(void *userdata, const OSVR_TimeValue *timestamp,
                             const struct OSVR_AnalogReport *report) {
    reportReceived = true;
}

TEST_CASE("BasicJointClientKitWithInterface-ConstructDestruct") {
    auto options = osvrJointClientCreateOptions();
    REQUIRE_FALSE(nullptr == options);

    REQUIRE(OSVR_RETURN_SUCCESS == osvrJointClientOptionsLoadPlugin(
                                       options, "com_osvr_example_AnalogSync"));

    REQUIRE(OSVR_RETURN_SUCCESS ==
            osvrJointClientOptionsTriggerHardwareDetect(options));

    auto ctx = osvrJointClientInit("org.osvr.test.jointclientkit", options);
    REQUIRE_FALSE(nullptr == ctx);
    REQUIRE(OSVR_RETURN_SUCCESS == osvrClientUpdate(ctx));

    OSVR_ClientInterface eye = nullptr;
    REQUIRE(
        OSVR_RETURN_SUCCESS ==
        osvrClientGetInterface(
            ctx, "/com_osvr_example_AnalogSync/MySyncDevice/analog/0", &eye));

    REQUIRE(OSVR_RETURN_SUCCESS ==
            osvrRegisterAnalogCallback(eye, &myAnalogCallback, nullptr));

    REQUIRE(OSVR_RETURN_SUCCESS == osvrClientUpdate(ctx));

    REQUIRE(reportReceived);

    REQUIRE(OSVR_RETURN_SUCCESS == osvrClientShutdown(ctx));
}
