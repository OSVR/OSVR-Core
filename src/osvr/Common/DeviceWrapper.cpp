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
#include "DeviceWrapper.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    DeviceWrapper::DeviceWrapper(std::string const &name,
                                 vrpn_ConnectionPtr const &conn, bool client)
        : vrpn_BaseClass(name.c_str(), conn.get()), m_conn(conn),
          m_client(client) {
        vrpn_BaseClass::init();
        m_setup(conn, common::RawSenderType(d_sender_id));

        // Clients: don't print "haven't heard from server" messages.
        if (client) {
            shutup = true;
        }
    }

    DeviceWrapper::~DeviceWrapper() {}

    void DeviceWrapper::mainloop() { update(); }

    void DeviceWrapper::m_update() {
        if (m_client) {
            client_mainloop();
            m_getConnection()->mainloop();
        } else {
            server_mainloop();
        }
    }

    int DeviceWrapper::register_types() {
        return 0; // success
    }
} // namespace common
} // namespace osvr
