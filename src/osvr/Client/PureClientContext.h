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
#include <osvr/Client/ClientContext.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/PathTree.h>
#include "ResolveTreeNode.h"
#include "VRPNConnectionCollection.h"
#include "InterfaceTree.h"
#include "RemoteHandlerFactory.h"

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>
#include <unordered_map>

namespace osvr {
namespace client {

    class PureClientContext : public ::OSVR_ClientContextObject {
      public:
        PureClientContext(const char appId[], const char host[] = "localhost");
        virtual ~PureClientContext();

      private:
        void m_setupDummyTree();
        virtual void m_update();
        virtual void m_sendRoute(std::string const &route);
        virtual void m_handleNewInterface(ClientInterfacePtr const &iface);
        virtual void
        m_handleReleasingInterface(ClientInterfacePtr const &iface);

        void m_connectCallbacksOnPath(std::string const &path);
        void m_removeCallbacksOnPath(std::string const &path);
        std::string m_host;
        vrpn_ConnectionPtr m_mainConn;
        common::PathTree m_pathTree;
        common::BaseDevicePtr m_systemDevice;
        common::SystemComponent *m_systemComponent;
        VRPNConnectionCollection m_vrpnConns;
        InterfaceTree m_interfaces;
        RemoteHandlerFactory m_factory;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_PureClientContext_h_GUID_0A40DCCB_0451_4DB0_855B_7ECE66C52D07
