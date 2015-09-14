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
#include <osvr/JointClientKit/JointClientKitC.h>
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>

// Library/third-party includes
// - none

// Standard includes
#include "gtest/gtest.h"

static bool reportReceived = false;

static void myEyeTracker2DCallback(void *userdata, const OSVR_TimeValue *timestamp,
                           const struct OSVR_EyeTracker2DReport *report) {
    reportReceived = true;
}

TEST(BasicJointClientKitWithInterface, ConstructDestruct) {
    auto options = osvrJointClientCreateOptions();
    ASSERT_NE(nullptr, options);

    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrJointClientOptionsLoadPlugin(options, "com_osvr_example_EyeTracker"));

    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrJointClientOptionsTriggerHardwareDetect(options));

    auto ctx = osvrJointClientInit("org.osvr.test.jointclientkit", options);
    ASSERT_NE(nullptr, ctx);
    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrClientUpdate(ctx));

    OSVR_ClientInterface eye = nullptr;
    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrClientGetInterface(ctx, "/me/eyes/left", &eye));

    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrRegisterEyeTracker2DCallback(eye, &myEyeTracker2DCallback, nullptr));

    for(int i = 0; i < 100; i++) {
      ASSERT_EQ(OSVR_RETURN_SUCCESS,
        osvrClientUpdate(ctx));
    }

    ASSERT_EQ(true, reportReceived);

    ASSERT_EQ(OSVR_RETURN_SUCCESS,
      osvrClientShutdown(ctx));
}
