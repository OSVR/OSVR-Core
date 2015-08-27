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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "PureClientContext.h"
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

#include <boost/algorithm/string.hpp>

// Library/third-party includes
#include <json/reader.h>

// Standard includes
#include <unordered_set>
#include <thread>

namespace osvr {
namespace client {

    class LocalhostReplacer : public boost::static_visitor<>,
                              boost::noncopyable {
      public:
        LocalhostReplacer(const std::string &host)
            : boost::static_visitor<>(), m_host(host) {
            BOOST_ASSERT_MSG(
                m_host.length() > 0,
                "Cannot replace localhost with an empty host name!");
        }

        /// @brief Replace localhost with proper hostname:port for Device
        /// elements
        void operator()(osvr::common::PathNode &,
                        osvr::common::elements::DeviceElement &elt) {
            static const auto LOCALHOST = "localhost";
            std::string &server = elt.getServer();

            auto it = server.find(LOCALHOST);

            if (it != server.npos) {
                // Do a bit of surgery, only the "localhost" must be replaced,
                // keeping the ":xxxx" part with the port number - the host
                // could be running a local VRPN/OSVR service on another port!

                // We have to do it like this, because std::string::replace()
                // has a silly undefined corner case when the string we are
                // replacing localhost with is shorter than the length of string
                // being replaced (see
                // http://www.cplusplus.com/reference/string/string/replace/ )
                // Better be safe than sorry :(

                server = boost::algorithm::ireplace_first_copy(
                    server, LOCALHOST,
                    m_host); // Go through a copy, just to be extra safe
            }
        }

        /// @brief Catch-all for other element types.
        template <typename T> void operator()(osvr::common::PathNode &, T &) {}

      protected:
        const std::string m_host;
    };

    static const std::chrono::milliseconds STARTUP_CONNECT_TIMEOUT(200);
    static const std::chrono::milliseconds STARTUP_TREE_TIMEOUT(1000);
    static const std::chrono::milliseconds STARTUP_LOOP_SLEEP(1);

    PureClientContext::PureClientContext(const char appId[], const char host[],
                                         common::ClientContextDeleter del)
        : ::OSVR_ClientContextObject(appId, del), m_host(host) {

        if (!m_network.isUp()) {
            throw std::runtime_error("Network error: " + m_network.getError());
        }

        /// Create all the remote handler factories.
        populateRemoteHandlerFactory(m_factory, m_vrpnConns);

        std::string sysDeviceName =
            std::string(common::SystemComponent::deviceName()) + "@" + host;
        m_mainConn = m_vrpnConns.getConnection(
            common::SystemComponent::deviceName(), host);

        /// Create the system client device.
        m_systemDevice = common::createClientDevice(sysDeviceName, m_mainConn);
        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
#define OSVR_USE_DEDUP
#ifdef OSVR_USE_DEDUP
        typedef common::DeduplicatingFunctionWrapper<Json::Value const &>
            DedupJsonFunction;
        m_systemComponent->registerReplaceTreeHandler(DedupJsonFunction(
            [&](Json::Value const &nodes) { m_handleReplaceTree(nodes); }));
#else
        // Just for testing purposes, figuring out why we end up looping too
        // much.
        m_systemComponent->registerReplaceTreeHandler(
            [&](Json::Value const &nodes, util::time::TimeValue const &) {
                m_handleReplaceTree(nodes);
            });
#endif
        typedef std::chrono::system_clock clock;
        auto begin = clock::now();

        // Spin the update to get a connection
        auto connEnd = begin + STARTUP_CONNECT_TIMEOUT;
        while (clock::now() < connEnd && !m_gotConnection) {
            m_update();
            std::this_thread::sleep_for(STARTUP_LOOP_SLEEP);
        }
        if (!m_gotConnection) {
            OSVR_DEV_VERBOSE(
                "Could not connect to OSVR server in the timeout period "
                "allotted of "
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       STARTUP_CONNECT_TIMEOUT)
                       .count()
                << "ms");
            return; // Bail early if we don't even have a connection
        }

        // Spin the update to get a path tree
        auto treeEnd = begin + STARTUP_TREE_TIMEOUT;
        while (clock::now() < treeEnd && !m_gotTree) {
            m_update();
            std::this_thread::sleep_for(STARTUP_LOOP_SLEEP);
        }
        auto timeToStartup = (clock::now() - begin);
        OSVR_DEV_VERBOSE(
            "Connection process took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   timeToStartup)
                   .count()
            << "ms: " << (m_gotConnection ? "have connection to server, "
                                          : "don't have connection to server, ")
            << (m_gotTree ? "have path tree" : "don't have path tree"));
    }

    PureClientContext::~PureClientContext() {}

    void PureClientContext::m_update() {
        /// Mainloop connections
        m_vrpnConns.updateAll();

        if (!m_gotConnection && m_mainConn->connected()) {
            OSVR_DEV_VERBOSE("Got connection to main OSVR server");
            m_gotConnection = true;
        }

        /// Update system device
        m_systemDevice->update();
        /// Update handlers.
        m_interfaces.updateHandlers();
    }

    void PureClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void PureClientContext::m_handleNewInterface(
        common::ClientInterfacePtr const &iface) {
        bool isNew = m_interfaces.addInterface(iface);
        if (isNew) {
            m_connectCallbacksOnPath(iface->getPath());
        }
    }

    void PureClientContext::m_handleReleasingInterface(
        common::ClientInterfacePtr const &iface) {
        bool isEmpty = m_interfaces.removeInterface(iface);
        if (isEmpty) {
            m_removeCallbacksOnPath(iface->getPath());
        }
    }

    common::PathTree const &PureClientContext::m_getPathTree() const {
        return m_pathTree;
    }

    bool PureClientContext::m_connectCallbacksOnPath(std::string const &path) {
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

    void PureClientContext::m_removeCallbacksOnPath(std::string const &path) {
        m_interfaces.eraseHandlerForPath(path);
    }

    void PureClientContext::m_connectNeededCallbacks() {
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

    void PureClientContext::m_handleReplaceTree(Json::Value const &nodes) {
        m_gotTree = true;
        OSVR_DEV_VERBOSE("Got updated path tree, processing");
        // reset path tree
        m_pathTree.reset();
        // wipe out handlers in the interface tree
        m_interfaces.clearHandlers();

        // populate path tree from message
        common::jsonToPathTree(m_pathTree, nodes);

        // replace the @localhost with the correct host name
        // in case we are a remote client, otherwise the connection
        // would fail

        LocalhostReplacer replacer(m_host);
        util::traverseWith(m_pathTree.getRoot(),
                           [&replacer](osvr::common::PathNode &node) {
                               common::applyPathNodeVisitor(replacer, node);
                           });

        // re-connect handlers.
        m_connectNeededCallbacks();
    }

} // namespace client
} // namespace osvr
