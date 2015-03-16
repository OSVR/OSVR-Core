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
#include "WiringTracker.h"
#include <osvr/Client/ResolveTreeNode.h>
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
        WiringTracker::createAndAddFactory(m_vrpnConns, m_wiringFactory);
        m_setupDummyTree();
    }

    PureClientContext::~PureClientContext() {}

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
    }

    void PureClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

    void
    PureClientContext::m_handleNewInterface(ClientInterfacePtr const &iface) {
        m_connectCallbacksOnInterface(iface);
    }

    void PureClientContext::m_connectCallbacksOnInterface(
        ClientInterfacePtr const &iface) {

        auto &node = m_pathTree.getNodeByPath(iface->getPath());
        auto handler = traverseRoute(m_pathTree, node, iface, m_wiringFactory);
        if (handler) {
            OSVR_DEV_VERBOSE("Successfully resolved handler for "
                             << iface->getPath());
        } else {
            OSVR_DEV_VERBOSE("Could not resolve handler for "
                             << iface->getPath());
        }
        iface->data() = handler;

        auto serializedTree = common::pathTreeToJson(m_pathTree);
        OSVR_DEV_VERBOSE(serializedTree.toStyledString());

        common::PathTree newTree;
        common::jsonToPathTree(newTree, serializedTree);

        auto newTreeSerialized = common::pathTreeToJson(newTree);
        OSVR_DEV_VERBOSE(newTreeSerialized.toStyledString());

        OSVR_DEV_VERBOSE(
            ((newTreeSerialized == serializedTree) ? "same" : "different"));
    }
} // namespace client
} // namespace osvr