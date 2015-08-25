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
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/NetworkingSupport.h>
#include <osvr/Util/TimeValue_fwd.h>
#include <osvr/Util/DefaultBool.h>
#include "VRPNConnectionCollection.h"
#include <osvr/Client/InterfaceTree.h>
#include "RemoteHandlerFactory.h"

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <json/value.h>

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

      private:
        virtual void m_update();
        virtual void m_sendRoute(std::string const &route);

        /// @brief Called with each new interface object before it is returned
        /// to the client.
        virtual void
        m_handleNewInterface(common::ClientInterfacePtr const &iface);

        /// @brief Called with each interface object to be released/deleted
        /// after it is removed from the context's list of interfaces but before
        /// it is deleted.
        virtual void
        m_handleReleasingInterface(common::ClientInterfacePtr const &iface);

        virtual common::PathTree const &m_getPathTree() const;

        /// @brief Given a path, remove any existing handler for that path, then
        /// attempt to fully resolve the path to its source and construct a
        /// handler for it.
        ///
        /// @return true if we were able to connect the path.
        bool m_connectCallbacksOnPath(std::string const &path);

        /// @brief Given a path, remove any existing handler for that path from
        /// both the handler container and the interface tree.
        void m_removeCallbacksOnPath(std::string const &path);

        /// @brief Given a JSON array of nodes representing a path tree, wipe
        /// out existing path tree and remote handlers, build new path tree, and
        /// set up new remote handlers for all known interfaces.
        void m_handleReplaceTree(Json::Value const &nodes);

        /// @brief Calls m_connectCallbacksOnPath() for every path that has one
        /// or more interface objects but no remote handler.
        void m_connectNeededCallbacks();

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

        /// @brief Path tree
        common::PathTree m_pathTree;

        /// @brief Tree parallel to path tree for holding interface objects and
        /// remote handlers.
        InterfaceTree m_interfaces;

        /// @brief Factory for producing remote handlers.
        RemoteHandlerFactory m_factory;

        /// @brief RAII holder for networking start/stop
        common::NetworkingSupport m_network;

        /// @brief Have we gotten a connection to the main server?
        util::DefaultBool<false> m_gotConnection;

        /// @brief Have we gotten a path tree?
        util::DefaultBool<false> m_gotTree;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_PureClientContext_h_GUID_0A40DCCB_0451_4DB0_855B_7ECE66C52D07
