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
#include "VRPNConnectionCollection.h"

// Library/third-party includes
#include <vrpn_Connection.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    VRPNConnectionCollection::VRPNConnectionCollection()
        : m_connMap(make_shared<ConnectionMap>()) {}

    vrpn_ConnectionPtr VRPNConnectionCollection::getConnection(
        common::elements::DeviceElement const &elt) {
        return getConnection(elt.getDeviceName(), elt.getServer());
    }

    vrpn_ConnectionPtr
    VRPNConnectionCollection::addConnection(vrpn_ConnectionPtr conn,
                                            std::string const &host) {
        auto connMap = *m_connMap;
        auto existing = connMap.find(host);
        if (existing != end(connMap)) {
            return existing->second;
        }
        connMap[host] = conn;
        return conn;
    }

    vrpn_ConnectionPtr
    VRPNConnectionCollection::getConnection(std::string const &device,
                                            std::string const &host) {
        auto connMap = *m_connMap;
        auto existing = connMap.find(host);
        if (existing != end(connMap)) {
            return existing->second;
        }
        auto fullName = device + "@" + host;

        vrpn_ConnectionPtr newConn(
            vrpn_get_connection_by_name(fullName.c_str(), nullptr, nullptr,
                                        nullptr, nullptr, nullptr, true));
        connMap[host] = newConn;
        newConn->removeReference(); // Remove extra reference.
        return newConn;
    }

    void VRPNConnectionCollection::updateAll() {
        for (auto &connPair : *m_connMap) {
            connPair.second->mainloop();
        }
    }

} // namespace client
} // namespace osvr
