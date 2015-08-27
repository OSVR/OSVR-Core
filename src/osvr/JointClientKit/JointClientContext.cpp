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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "JointClientContext.h"
#include <osvr/Common/SystemComponent.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Common/DeduplicatingFunctionWrapper.h>
#include <osvr/Connection/Connection.h>
#include <osvr/Server/Server.h>

// Library/third-party includes
#include <json/reader.h>

// Standard includes
#include <unordered_set>
#include <thread>

namespace osvr {
namespace client {

    static const std::chrono::milliseconds STARTUP_CONNECT_TIMEOUT(200);
    static const std::chrono::milliseconds STARTUP_TREE_TIMEOUT(1000);
    static const std::chrono::milliseconds STARTUP_LOOP_SLEEP(1);

    JointClientContext::JointClientContext(const char appId[],
                                           common::ClientContextDeleter del)
        : ::OSVR_ClientContextObject(appId, del), m_host("localhost") {

        /// Create all the remote handler factories.
        populateRemoteHandlerFactory(m_factory, m_vrpnConns);

        /// creates the OSVR connection with its nested VRPN connection
        auto conn = connection::Connection::createLoopbackConnection();

        /// Get the VRPN connection out and use it.
        m_mainConn = static_cast<vrpn_Connection *>(std::get<0>(conn));
        m_vrpnConns.addConnection(m_mainConn, m_host);

        /// Get the OSVR connection out and use it to make a server.
        m_server = server::Server::create(std::get<1>(conn));

        std::string sysDeviceName =
            std::string(common::SystemComponent::deviceName()) + "@" + m_host;

        /// Create the system client device.
        m_systemDevice = common::createClientDevice(sysDeviceName, m_mainConn);
        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
        typedef common::DeduplicatingFunctionWrapper<Json::Value const &>
            DedupJsonFunction;
        m_systemComponent->registerReplaceTreeHandler(DedupJsonFunction(
            [&](Json::Value const &nodes) { m_handleReplaceTree(nodes); }));
    }

    JointClientContext::~JointClientContext() {}

    void JointClientContext::m_update() {
        /// Run the server
        m_server->update();

        /// Mainloop connections
        m_vrpnConns.updateAll();

        /// Update system device
        m_systemDevice->update();
        /// Update handlers.
        m_interfaces.updateHandlers();
    }

    void JointClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void JointClientContext::m_handleNewInterface(
        common::ClientInterfacePtr const &iface) {
        bool isNew = m_interfaces.addInterface(iface);
        if (isNew) {
            m_connectCallbacksOnPath(iface->getPath());
        }
    }

    void JointClientContext::m_handleReleasingInterface(
        common::ClientInterfacePtr const &iface) {
        bool isEmpty = m_interfaces.removeInterface(iface);
        if (isEmpty) {
            m_removeCallbacksOnPath(iface->getPath());
        }
    }

    common::PathTree const &JointClientContext::m_getPathTree() const {
        return m_pathTree;
    }

    bool JointClientContext::m_connectCallbacksOnPath(std::string const &path) {
        /// Start by removing handler from interface tree and handler container
        /// for this path, if found. Ensures that if we early-out (fail to set
        /// up a handler) we don't have a leftover one still active.
        m_interfaces.eraseHandlerForPath(path);

        auto source = common::resolveTreeNode(m_pathTree, path);
        if (!source.is_initialized()) {
            OSVR_DEV_VERBOSE("Could not resolve source for " << path);
            return false;
        }
        auto handler = m_factory.invokeFactory(
            *source, m_interfaces.getInterfacesForPath(path), *this);
        if (handler) {
            OSVR_DEV_VERBOSE("Successfully produced handler for " << path);
            // Store the new handler in the interface tree
            auto oldHandler = m_interfaces.replaceHandlerForPath(path, handler);
            BOOST_ASSERT_MSG(
                !oldHandler,
                "We removed the old handler before so it should be null now");
            return true;
        }

        OSVR_DEV_VERBOSE("Could not produce handler for " << path);
        return false;
    }

    void JointClientContext::m_removeCallbacksOnPath(std::string const &path) {
        m_interfaces.eraseHandlerForPath(path);
    }

    void JointClientContext::m_connectNeededCallbacks() {
        std::unordered_set<std::string> failedPaths;
        size_t successfulPaths{0};
        for (auto const &iface : getInterfaces()) {
            /// @todo slightly overkill, but it works - tree traversal would be
            /// better.
            auto path = iface->getPath();
            /// For every interface, if there's no handler at that path on the
            /// interface tree, try to set one up.
            if (!m_interfaces.getHandlerForPath(path)) {
                auto success = m_connectCallbacksOnPath(path);
                if (success) {
                    successfulPaths++;
                } else {
                    failedPaths.insert(path);
                }
            }
        }
        OSVR_DEV_VERBOSE("Connected " << successfulPaths << " of "
                                      << successfulPaths + failedPaths.size()
                                      << " unconnected paths successfully");
    }

    void JointClientContext::m_handleReplaceTree(Json::Value const &nodes) {
        OSVR_DEV_VERBOSE("Got updated path tree, processing");
        // reset path tree
        m_pathTree.reset();
        // wipe out handlers in the interface tree
        m_interfaces.clearHandlers();

        // populate path tree from message
        common::jsonToPathTree(m_pathTree, nodes);

        // re-connect handlers.
        m_connectNeededCallbacks();
    }

} // namespace client
} // namespace osvr
