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
#include "AnalogRemoteFactory.h"
#include "ButtonRemoteFactory.h"
#include "ImagingRemoteFactory.h"
#include "TrackerRemoteFactory.h"
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/DeduplicatingFunctionWrapper.h>

// Library/third-party includes
#include <json/reader.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    PureClientContext::PureClientContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        if (!m_network.isUp()) {
            throw std::runtime_error("Network error: " + m_network.getError());
        }

        /// Register all the factories.
        TrackerRemoteFactory(m_vrpnConns).registerWith(m_factory);
        AnalogRemoteFactory(m_vrpnConns).registerWith(m_factory);
        ButtonRemoteFactory(m_vrpnConns).registerWith(m_factory);
        ImagingRemoteFactory(m_vrpnConns).registerWith(m_factory);

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
    }

    PureClientContext::~PureClientContext() {}

    void PureClientContext::m_update() {
        /// Mainloop connections
        m_vrpnConns.updateAll();
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

    void PureClientContext::m_connectCallbacksOnPath(std::string const &path) {
        /// Start by removing handler from interface tree and handler container
        /// for this path, if found. Ensures that if we early-out (fail to set
        /// up a handler) we don't have a leftover one still active.
        m_interfaces.eraseHandlerForPath(path);

        auto source = common::resolveTreeNode(m_pathTree, path);
        if (!source.is_initialized()) {
            OSVR_DEV_VERBOSE("Could not resolve source for " << path);
            return;
        }
        auto handler = m_factory.invokeFactory(
            *source, m_interfaces.getInterfacesForPath(path));
        if (handler) {
            OSVR_DEV_VERBOSE("Successfully produced handler for " << path);
            // Store the new handler in the interface tree
            auto oldHandler = m_interfaces.replaceHandlerForPath(path, handler);
            BOOST_ASSERT_MSG(
                !oldHandler,
                "We removed the old handler before so it should be null now");
        } else {
            OSVR_DEV_VERBOSE("Could not produce handler for " << path);
        }
    }

    void PureClientContext::m_removeCallbacksOnPath(std::string const &path) {
        m_interfaces.eraseHandlerForPath(path);
    }

    void PureClientContext::m_connectNeededCallbacks() {
        OSVR_DEV_VERBOSE(
            "*** Entering PureClientContext::m_connectNeededCallbacks");
        for (auto const &iface : getInterfaces()) {
            /// @todo slightly overkill, but it works - tree traversal would be
            /// better.
            auto path = iface->getPath();
            /// For every interface, if there's no handler at that path on the
            /// interface tree, try to set one up.
            if (!m_interfaces.getHandlerForPath(path)) {
                m_connectCallbacksOnPath(path);
            }
        }

        OSVR_DEV_VERBOSE(
            "*** Exiting PureClientContext::m_connectNeededCallbacks");
    }

    void PureClientContext::m_handleReplaceTree(Json::Value const &nodes) {
        OSVR_DEV_VERBOSE(
            "PureClientContext::m_handleConfigAddNodes - clearing tree");
        // reset path tree
        m_pathTree.reset();
        // wipe out handlers in the interface tree
        m_interfaces.clearHandlers();

        OSVR_DEV_VERBOSE("PureClientContext::m_handleConfigAddNodes - "
                         "repopulating and connecting tree");
        common::jsonToPathTree(m_pathTree, nodes);
        m_connectNeededCallbacks();
    }

    common::PathElement &
    PureClientContext::m_getElementByPath(std::string const &path) {
        return m_pathTree.getNodeByPath(path).value();
    }
} // namespace client
} // namespace osvr