/** @file
    @brief Header

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

#ifndef INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809
#define INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809

// Internal Includes
#include <osvr/Connection/Connection.h>
#include <osvr/Common/NetworkingSupport.h>

// Library/third-party includes
#include <vrpn_Connection.h>
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace osvr {
namespace connection {
    namespace messageid {
        /// @brief Return the string identifying VRPN ping messages
        const char *vrpnPing();
    } // namespace messageid
    class VrpnBasedConnection : public Connection {
      public:
        enum ConnectionType { VRPN_LOCAL_ONLY, VRPN_SHARED, VRPN_LOOPBACK };

        /// @brief Constructor for the VRPN connection.
        VrpnBasedConnection(ConnectionType type);

        /// @brief Constructor for a (likely) shared VRPN connection specifying
        /// the interface to listen on as well as the port.
        VrpnBasedConnection(boost::optional<std::string const &> iface,
                            boost::optional<int> port);

        /// @brief Returns the vrpn_Connection pointer.
        virtual void *getUnderlyingObject();
        virtual const char *getConnectionKindID();
        virtual ~VrpnBasedConnection();

      private:
        /// @brief Helper method to set up the VRPN server connection
        /// @param iface String specifying the interface to use. Null means all
        /// interfaces.
        /// @param port Port to listen on (both UDP and TCP). 0 means use the
        /// VRPN default.
        void m_initConnection(const char NIC[] = nullptr, int port = 0);

        virtual MessageTypePtr
        m_registerMessageType(std::string const &messageId);
        virtual ConnectionDevicePtr
        m_createConnectionDevice(DeviceInitObject &init);
        virtual void m_registerConnectionHandler(std::function<void()> handler);
        virtual void m_process();

        static int VRPN_CALLBACK m_connectionHandler(void *userdata,
                                                     vrpn_HANDLERPARAM);

        vrpn_ConnectionPtr m_vrpnConnection;
        std::vector<std::function<void()> > m_connectionHandlers;
        common::NetworkingSupport m_network;
    };

} // namespace connection
} // namespace osvr
#endif // INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809
