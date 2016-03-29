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
#include <osvr/Common/SkeletonComponent.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/DeduplicatingFunctionWrapper.h>
#include "SkeletonRemoteFactory.h"
#include <boost/algorithm/string.hpp>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <unordered_set>
#include <thread>

namespace osvr {
namespace client {
    inline void replaceLocalhostServers(Json::Value &nodes,
                                        std::string const &host) {
        BOOST_ASSERT_MSG(host.length() > 0,
                         "Cannot replace localhost with an empty host name!");
        const auto deviceElementTypeName =
            common::elements::getTypeName<common::elements::DeviceElement>();
        static const auto LOCALHOST = "localhost";
        for (auto &node : nodes) {
            if (node["type"].asString() == deviceElementTypeName) {
                auto &serverRef = node["server"];
                auto server = serverRef.asString();

                auto it = server.find(LOCALHOST);

                if (it != server.npos) {
                    // Do a bit of surgery, only the "localhost" must be
                    // replaced, keeping the ":xxxx" part with the port number
                    // (or even the potential "tcp://" prefix) - the host could
                    // be running a local VRPN/OSVR service on another port!

                    // We have to do it like this, because
                    // std::string::replace() has a silly undefined corner case
                    // when the string we are replacing localhost with is
                    // shorter than the length of string being replaced (see
                    // http://www.cplusplus.com/reference/string/string/replace/
                    // )
                    // Better be safe than sorry :(

                    serverRef = boost::algorithm::ireplace_first_copy(
                        server, LOCALHOST,
                        host); // Go through a copy, just to be extra safe
                }
            }
        }
    }

    static const std::chrono::milliseconds STARTUP_CONNECT_TIMEOUT(200);
    static const std::chrono::milliseconds STARTUP_TREE_TIMEOUT(1000);
    static const std::chrono::milliseconds STARTUP_LOOP_SLEEP(1);

    PureClientContext::PureClientContext(const char appId[], const char host[],
                                         common::ClientContextDeleter del)
        : ::OSVR_ClientContextObject(appId, del), m_host(host),
          m_ifaceMgr(m_pathTreeOwner, m_factory,
                     *static_cast<common::ClientContext *>(this)) {

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
        using DedupJsonFunction =
            common::DeduplicatingFunctionWrapper<Json::Value const &>;

        m_systemComponent->registerReplaceTreeHandler(
            DedupJsonFunction([&](Json::Value nodes) {
                logger()->debug("Got updated path tree, processing");
                // Replace localhost before we even convert the json to a tree.
                // replace the @localhost with the correct host name
                // in case we are a remote client, otherwise the connection
                // would fail
                replaceLocalhostServers(nodes, m_host);

                // Tree observers will handle destruction/creation of remote
                // handlers.
                m_pathTreeOwner.replaceTree(nodes);
            }));

        typedef std::chrono::system_clock clock;
        auto begin = clock::now();

        // Spin the update to get a connection
        auto connEnd = begin + STARTUP_CONNECT_TIMEOUT;
        while (clock::now() < connEnd && !m_gotConnection) {
            m_update();
            std::this_thread::sleep_for(STARTUP_LOOP_SLEEP);
        }
        if (!m_gotConnection) {
            logger()->notice()
                << "Could not connect to OSVR server in the timeout period "
                   "allotted of "
                << std::chrono::duration_cast<std::chrono::milliseconds>(
                       STARTUP_CONNECT_TIMEOUT)
                       .count()
                << "ms";
            return; // Bail early if we don't even have a connection
        }

        // Spin the update to get a path tree
        auto treeEnd = begin + STARTUP_TREE_TIMEOUT;
        while (clock::now() < treeEnd && !m_pathTreeOwner) {
            m_update();
            std::this_thread::sleep_for(STARTUP_LOOP_SLEEP);
        }
        auto timeToStartup = (clock::now() - begin);

        // this message is just "info" if we're all good, but "notice" if we
        // aren't fully set up yet.
        logger()->log(m_pathTreeOwner ? util::log::LogLevel::info
                                      : util::log::LogLevel::notice)
            << "Connection process took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   timeToStartup)
                   .count()
            << "ms: " << (m_gotConnection ? "have connection to server, "
                                          : "don't have connection to server, ")
            << (m_pathTreeOwner ? "have path tree" : "don't have path tree");
    }

    PureClientContext::~PureClientContext() {}

    void PureClientContext::m_update() {
        /// Mainloop connections
        m_vrpnConns.updateAll();

        if (!m_gotConnection && m_mainConn->connected()) {
            logger()->info("Got connection to main OSVR server");
            m_gotConnection = true;
        }

        /// Update system device
        m_systemDevice->update();
        /// Update handlers.
        m_ifaceMgr.updateHandlers();
    }

    void PureClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void PureClientContext::m_handleNewInterface(
        common::ClientInterfacePtr const &iface) {
        m_ifaceMgr.addInterface(iface);
    }

    void PureClientContext::m_handleReleasingInterface(
        common::ClientInterfacePtr const &iface) {
        m_ifaceMgr.releaseInterface(iface);
    }

    bool PureClientContext::m_getStatus() const {
        return m_gotConnection && m_pathTreeOwner;
    }

    common::PathTree const &PureClientContext::m_getPathTree() const {
        return m_pathTreeOwner.get();
    }

    common::Transform const &
    PureClientContext::m_getRoomToWorldTransform() const {
        return m_roomToWorld;
    }

    void PureClientContext::m_setRoomToWorldTransform(
        common::Transform const &xform) {
        m_roomToWorld = xform;
    }

    RemoteHandlerPtr
    PureClientContext::m_getRemoteHandler(std::string const &path) {
        return m_ifaceMgr.getRemoteHandlerForPath(path);
    }

    /// @brief Articulation Tree corresponding to path
    common::PathTree const &PureClientContext::m_getArticulationTree(
        std::string const &path) {
        // get a handler for path, should be skeleton handler
        auto handler = m_getRemoteHandler(path);
        // cast it to skeleton handler
        std::shared_ptr<SkeletonRemoteHandler> skeletonHandler =
            std::dynamic_pointer_cast<SkeletonRemoteHandler>(handler);
        auto skeletonComp = skeletonHandler->getSkeletonComponent();

        return skeletonComp->getArticulationTree();
    }
} // namespace client
} // namespace osvr
