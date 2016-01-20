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
#include <osvr/Common/CommonComponent.h>
#include <vrpn_Connection.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

namespace messages = osvr::common::messages;

TEST(CommonComponentMessages, GotFirstConnection) {
    ASSERT_STREQ(messages::VRPNGotFirstConnection::identifier(),
                 vrpn_got_first_connection);
}

TEST(CommonComponentMessages, GotConnection) {
    ASSERT_STREQ(messages::VRPNGotConnection::identifier(),
                 vrpn_got_connection);
}

TEST(CommonComponentMessages, DroppedConnection) {
    ASSERT_STREQ(messages::VRPNDroppedConnection::identifier(),
                 vrpn_dropped_connection);
}

TEST(CommonComponentMessages, DroppedLastConnection) {
    ASSERT_STREQ(messages::VRPNDroppedLastConnection::identifier(),
                 vrpn_dropped_last_connection);
}
