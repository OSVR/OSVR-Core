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
#include "VrpnBasedConnection.h"
#include "VrpnMessageType.h"
#include "VrpnConnectionDevice.h"
#include "VrpnConnectionKind.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    namespace messageid {
        /// @brief Return the string identifying VRPN ping messages
        const char *vrpnPing() { return "vrpn_Base ping_message"; }
    } // namespace messageid

    VrpnBasedConnection::VrpnBasedConnection(ConnectionType type) {
        switch (type) {
        case VRPN_LOCAL_ONLY: {
            m_initConnection("localhost");
            break;
        }
        case VRPN_SHARED: {
            m_initConnection();
            break;
        }
        case VRPN_LOOPBACK: {
            m_initConnection("loopback:");
            break;
        }
        }
    }

    VrpnBasedConnection::VrpnBasedConnection(
        boost::optional<std::string const &> iface, boost::optional<int> port) {
        int myPort = port.get_value_or(0);
        if (iface && !(iface->empty())) {
            m_initConnection(iface->c_str(), myPort);
        } else {
            m_initConnection(nullptr, myPort);
        }
    }

    void VrpnBasedConnection::m_initConnection(const char iface[], int port) {
        if (!m_network.isUp()) {
            OSVR_DEV_VERBOSE("Network error: " << m_network.getError());
            throw std::runtime_error(m_network.getError());
        }
        if (0 == port) {
            port = vrpn_DEFAULT_LISTEN_PORT_NO;
        }
        m_vrpnConnection = vrpn_ConnectionPtr::create_server_connection(
            port, nullptr, nullptr, iface);
    }

    MessageTypePtr
    VrpnBasedConnection::m_registerMessageType(std::string const &messageId) {
        MessageTypePtr ret(new VrpnMessageType(messageId, m_vrpnConnection));
        return ret;
    }

    ConnectionDevicePtr
    VrpnBasedConnection::m_createConnectionDevice(DeviceInitObject &init) {
        ConnectionDevicePtr ret =
            make_shared<VrpnConnectionDevice>(init, m_vrpnConnection);
        return ret;
    }

    void VrpnBasedConnection::m_registerConnectionHandler(
        std::function<void()> handler) {
        if (m_connectionHandlers.empty()) {
            /// got connection handler
            m_vrpnConnection->register_handler(
                m_vrpnConnection->register_message_type(vrpn_got_connection),
                &m_connectionHandler, static_cast<void *>(this),
                vrpn_ANY_SENDER);
/// Ping handler
#if 0
            m_vrpnConnection->register_handler(
                m_vrpnConnection->register_message_type(messageid::vrpnPing()),
                &m_connectionHandler, static_cast<void *>(this),
                vrpn_ANY_SENDER);
#endif
        }
        m_connectionHandlers.push_back(handler);
    }

    int VrpnBasedConnection::m_connectionHandler(void *userdata,
                                                 vrpn_HANDLERPARAM) {
        VrpnBasedConnection *conn =
            static_cast<VrpnBasedConnection *>(userdata);
        for (auto const &f : conn->m_connectionHandlers) {
            f();
        }
        return 0;
    }
    void VrpnBasedConnection::m_process() { m_vrpnConnection->mainloop(); }

    VrpnBasedConnection::~VrpnBasedConnection() {
        /// @todo wait until all async threads are done
    }

    void *VrpnBasedConnection::getUnderlyingObject() {
        return static_cast<void *>(m_vrpnConnection.get());
    }

    const char *VrpnBasedConnection::getConnectionKindID() {
        return getVRPNConnectionKindID();
    }

} // namespace connection
} // namespace osvr
