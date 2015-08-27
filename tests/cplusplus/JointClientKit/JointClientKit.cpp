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

// Library/third-party includes
// - none

// Standard includes
#include "gtest/gtest.h"

TEST(BasicJointClientKit, ConstructDestruct) {
    auto ctx = osvrJointClientInit("org.osvr.test.jointclientkit", nullptr);
    ASSERT_NE(nullptr, ctx);
    ASSERT_EQ(OSVR_RETURN_SUCCESS, osvrClientUpdate(ctx));
    ASSERT_EQ(OSVR_RETURN_SUCCESS, osvrClientShutdown(ctx));
}
