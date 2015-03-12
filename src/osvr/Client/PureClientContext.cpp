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

// Library/third-party includes
#include <osvr/Common/SystemComponent.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    PureClientContext::PureClientContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {
        std::string sysDeviceName =
            std::string(common::SystemComponent::deviceName()) + "@" + host;
        m_mainConn =
            vrpn_get_connection_by_name(sysDeviceName.c_str(), nullptr, nullptr,
                                        nullptr, nullptr, nullptr, true);
        m_mainConn->removeReference(); // Remove extra reference.
        m_vrpnConns[m_host] = m_mainConn;

        /// Create the system client device.
        m_systemDevice = common::createClientDevice(sysDeviceName, m_mainConn);
        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
#if 0
            m_systemComponent->registerRoutesHandler(
                &VRPNContext::m_handleRoutingMessage, static_cast<void *>(this));
#endif
    }
    PureClientContext::~PureClientContext() {}
    void PureClientContext::m_setupDummyTree() {
        m_pathTree.getNodeByPath("/org_opengoggles_bundled_Multiserver",
                                 common::elements::PluginElement());
    }
    void PureClientContext::m_update() {
        /// Mainloop connections
        for (auto &connPair : m_vrpnConns) {
            connPair.second->mainloop();
        }
    }
    void PureClientContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
        m_update();
    }

} // namespace client
} // namespace osvr