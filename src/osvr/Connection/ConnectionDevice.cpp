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
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Connection/DeviceToken.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {

    ConnectionDevice::~ConnectionDevice() {}

    std::string const &ConnectionDevice::getName() const {
        return m_names.front();
    }

    ConnectionDevice::NameList const &ConnectionDevice::getNames() const {
        return m_names;
    }

    ConnectionDevice::ConnectionDevice(std::string const &name)
        : m_names(1, name), m_token(nullptr) {}

    ConnectionDevice::ConnectionDevice(ConnectionDevice::NameList const &names)
        : m_names(names), m_token(nullptr) {}

    void ConnectionDevice::process() { m_process(); }

    void ConnectionDevice::sendData(util::time::TimeValue const &timestamp,
                                    MessageType *type, const char *bytestream,
                                    size_t len) {
        BOOST_ASSERT(type);
        m_sendData(timestamp, type, bytestream, len);
    }

    void ConnectionDevice::setDeviceToken(DeviceToken &token) {
        BOOST_ASSERT(m_token == nullptr);
        m_token = &token;
    }

    bool ConnectionDevice::m_hasDeviceToken() const {
        return m_token != nullptr;
    }

    DeviceToken &ConnectionDevice::m_getDeviceToken() {
        BOOST_ASSERT(m_token);
        return *m_token;
    }

} // namespace connection
} // namespace osvr
