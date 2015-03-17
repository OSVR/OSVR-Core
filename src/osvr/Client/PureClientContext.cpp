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
#include <osvr/Client/ClientInterface.h>
#include "TrackerRemoteFactory.h"
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes

// Standard includes
// - none

namespace osvr {
namespace client {

    PureClientContext::PureClientContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {
        std::string sysDeviceName =
            std::string(common::SystemComponent::deviceName()) + "@" + host;
        m_mainConn = m_vrpnConns.getConnection(
            common::SystemComponent::deviceName(), host);

        /// Create the system client device.
        m_systemDevice = common::createClientDevice(sysDeviceName, m_mainConn);
        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
#if 0
            m_systemComponent->registerRoutesHandler(
                &VRPNContext::m_handleRoutingMessage, static_cast<void *>(this));
#endif

        auto vrpnConns = m_vrpnConns;
        TrackerRemoteFactory::createAndAddFactory(m_vrpnConns, m_factory);
        m_setupDummyTree();
    }

    PureClientContext::~PureClientContext() {
        for (auto const &iface : getInterfaces()) {
            m_handleReleasingInterface(iface);
        }
    }

    void PureClientContext::m_setupDummyTree() {
        m_pathTree.getNodeByPath("/org_opengoggles_bundled_Multiserver",
                                 common::elements::PluginElement());
        m_pathTree.getNodeByPath(
            "/org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
            common::elements::DeviceElement::createVRPNDeviceElement(
                "org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
                "localhost"));
        m_pathTree.getNodeByPath(
            "/org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0/tracker",
            common::elements::InterfaceElement());

        m_pathTree.getNodeByPath("/me/hands/left",
                                 common::elements::AliasElement(
                                     "/org_opengoggles_bundled_Multiserver/"
                                     "YEI_3Space_Sensor0/tracker/1"));
    }

    void PureClientContext::m_update() {
        /// Mainloop connections
        m_vrpnConns.updateAll();
        /// Update handlers.
        m_handlers.update();
    }

    void PureClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void
    PureClientContext::m_handleNewInterface(ClientInterfacePtr const &iface) {
        bool isNew = m_interfaces.addInterface(iface);
        if (isNew) {
            m_connectCallbacksOnPath(iface->getPath());
        }
    }

    void PureClientContext::m_handleReleasingInterface(
        ClientInterfacePtr const &iface) {
        bool isEmpty = m_interfaces.removeInterface(iface);
        if (isEmpty) {
            m_removeCallbacksOnPath(iface->getPath());
        }
    }

    void PureClientContext::m_connectCallbacksOnPath(std::string const &path) {
        auto source = common::resolveTreeNode(m_pathTree, path);
        if (!source.is_initialized()) {
            OSVR_DEV_VERBOSE("Could not resolve source for " << path);
            return;
        }
        auto handler = m_factory.invokeFactory(
            *source, m_interfaces.getInterfacesForPath(path));
        if (handler) {
            OSVR_DEV_VERBOSE("Successfully produced handler for " << path);
            // Add the new handler to our collection
            m_handlers.add(handler);
            // Replace the old handler if any in the interface tree,
            // and if we had an old handler remove it from our collection
            m_handlers.remove(m_interfaces.replaceHandlerForPath(path, handler));
        } else {
            OSVR_DEV_VERBOSE("Could not produce handler for " << path);
        }
    }
    void PureClientContext::m_removeCallbacksOnPath(std::string const &path) {
        m_handlers.remove(m_interfaces.eraseHandlerForPath(path));
    }
} // namespace client
} // namespace osvr