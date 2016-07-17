/** @file
    @brief Header

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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_PureClientContext_h_GUID_0A40DCCB_0451_4DB0_855B_7ECE66C52D07
#define INCLUDED_PureClientContext_h_GUID_0A40DCCB_0451_4DB0_855B_7ECE66C52D07

// Internal Includes
#include "VRPNConnectionCollection.h"
#include <osvr/Client/ClientInterfaceObjectManager.h>
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Client/RemoteHandlerFactory.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/NetworkingSupport.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathTreeOwner.h>
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/Transform.h>
#include <osvr/Util/TimeValue_fwd.h>

// Library/third-party includes
#include <json/value.h>
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>

namespace osvr {
namespace client {

    class PureClientContext : public ::OSVR_ClientContextObject {
      public:
        PureClientContext(const char appId[], common::ClientContextDeleter del)
            : PureClientContext(appId, "localhost", del) {}
        PureClientContext(const char appId[], const char host[],
                          common::ClientContextDeleter del);
        virtual ~PureClientContext();
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      private:
        void m_update() override;
        void m_sendRoute(std::string const &route) override;

        /// @brief Called with each new interface object before it is returned
        /// to the client.
        void
        m_handleNewInterface(common::ClientInterfacePtr const &iface) override;

        /// @brief Called with each interface object to be released/deleted
        /// after it is removed from the context's list of interfaces but before
        /// it is deleted.
        void m_handleReleasingInterface(
            common::ClientInterfacePtr const &iface) override;

        common::PathTree const &m_getPathTree() const override;

        common::Transform const &m_getRoomToWorldTransform() const override;

        void m_setRoomToWorldTransform(common::Transform const &xform) override;

        bool m_getStatus() const override;

        /// @brief The main OSVR server host: usually localhost
        std::string m_host;

        /// @brief the vrpn_Connection corresponding to m_host
        vrpn_ConnectionPtr m_mainConn;

        /// @brief The "OSVR" system device for control messages
        common::BaseDevicePtr m_systemDevice;

        /// @brief The system component providing access to sending/receiving
        /// control messages.
        common::SystemComponent *m_systemComponent;

        /// @brief All open VRPN connections, keyed by host
        VRPNConnectionCollection m_vrpnConns;

        /// @brief Object owning a path tree.
        common::PathTreeOwner m_pathTreeOwner;

        /// @brief Factory for producing remote handlers.
        RemoteHandlerFactory m_factory;

        /// @brief RAII holder for networking start/stop
        common::NetworkingSupport m_network;

        /// @brief Have we gotten a connection to the main server?
        bool m_gotConnection = false;

        /// @brief Room to world transform.
        common::Transform m_roomToWorld;

        /// @brief Manager of client interface objects and their interaction
        /// with the path tree.
        ClientInterfaceObjectManager m_ifaceMgr;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_PureClientContext_h_GUID_0A40DCCB_0451_4DB0_855B_7ECE66C52D07
