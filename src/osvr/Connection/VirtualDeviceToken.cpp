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
#include "VirtualDeviceToken.h"
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Util/GuardInterfaceDummy.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    VirtualDeviceToken::VirtualDeviceToken(std::string const &name)
        : OSVR_DeviceTokenObject(name) {}

    VirtualDeviceToken::~VirtualDeviceToken() {}

    void VirtualDeviceToken::m_setUpdateCallback(
        osvr::connection::DeviceUpdateCallback const &) {
        BOOST_ASSERT_MSG(0, "Should never be called - virtual device tokens "
                            "don't have typical update callbacks!");
    }

    void VirtualDeviceToken::m_sendData(util::time::TimeValue const &timestamp,
                                        MessageType *type,
                                        const char *bytestream, size_t len) {
        m_getConnectionDevice()->sendData(timestamp, type, bytestream, len);
    }

    util::GuardPtr VirtualDeviceToken::m_getSendGuard() {
        return util::GuardPtr(new util::DummyGuard);
    }

    void VirtualDeviceToken::m_connectionInteract() {}
} // namespace connection
} // namespace osvr
