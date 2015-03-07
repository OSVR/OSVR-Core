/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/VRPNServer/GetVRPNConnection.h>
#include "../Connection/VrpnConnectionKind.h" // Internal header!

// Library/third-party includes
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <vrpn_Connection.h>

// Standard includes
// - none

namespace osvr {
namespace vrpnserver {
    vrpn_Connection *getVRPNConnection(OSVR_PluginRegContext ctx) {
        return getVRPNConnection(
            osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx));
    }

    vrpn_Connection *getVRPNConnection(
        osvr::pluginhost::PluginSpecificRegistrationContext &context) {
        osvr::connection::ConnectionPtr conn =
            osvr::connection::Connection::retrieveConnection(
                context.getParent());
        vrpn_Connection *ret = nullptr;
        if (std::string(conn->getConnectionKindID()) ==
            osvr::connection::getVRPNConnectionKindID()) {
            ret = static_cast<vrpn_Connection *>(conn->getUnderlyingObject());
        }
        return ret;
    }
} // namespace vrpnserver
} // namespace osvr
