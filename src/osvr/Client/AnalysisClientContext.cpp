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
#include "AnalysisClientContext.h"
#include <osvr/Common/SystemComponent.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/DeduplicatingFunctionWrapper.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <unordered_set>
#include <thread>

namespace osvr {
namespace client {

    static const std::chrono::milliseconds STARTUP_CONNECT_TIMEOUT(200);
    static const std::chrono::milliseconds STARTUP_TREE_TIMEOUT(1000);
    static const std::chrono::milliseconds STARTUP_LOOP_SLEEP(1);

    AnalysisClientContext::AnalysisClientContext(
        const char appId[], const char host[], vrpn_ConnectionPtr const &conn,
        common::ClientContextDeleter del)
        : ::OSVR_ClientContextObject(appId, del), m_mainConn(conn),
          m_ifaceMgr(m_pathTreeOwner, m_factory,
                     *static_cast<common::ClientContext *>(this)) {

        /// Create all the remote handler factories.
        populateRemoteHandlerFactory(m_factory, m_vrpnConns);

        m_vrpnConns.addConnection(m_mainConn, "localhost");
        m_vrpnConns.addConnection(m_mainConn, host);
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

                OSVR_DEV_VERBOSE("Got updated path tree, processing");

                // Tree observers will handle destruction/creation of remote
                // handlers.
                m_pathTreeOwner.replaceTree(nodes);
            }));

        // No startup spin.
    }

    AnalysisClientContext::~AnalysisClientContext() {}

    void AnalysisClientContext::m_update() {
        m_started = true;
        /// Update system device
        m_systemDevice->update();
        /// Update handlers.
        m_ifaceMgr.updateHandlers();
    }

    void AnalysisClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void AnalysisClientContext::m_handleNewInterface(
        common::ClientInterfacePtr const &iface) {
        m_ifaceMgr.addInterface(iface, m_started);
    }

    void AnalysisClientContext::m_handleReleasingInterface(
        common::ClientInterfacePtr const &iface) {
        m_ifaceMgr.releaseInterface(iface);
    }

    bool AnalysisClientContext::m_getStatus() const {
        return bool(m_pathTreeOwner);
    }

    common::PathTree const &AnalysisClientContext::m_getPathTree() const {
        return m_pathTreeOwner.get();
    }
} // namespace client
} // namespace osvr
