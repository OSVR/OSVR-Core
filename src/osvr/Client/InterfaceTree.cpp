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
#include "InterfaceTree.h"
#include <osvr/Client/ClientInterface.h>
#include "../Common/PathParseAndRetrieve.h" /// @todo internal header cross-include

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>

namespace osvr {
namespace client {
    InterfaceTree::InterfaceTree() : m_root(node_type::createRoot()) {}

    bool InterfaceTree::addInterface(ClientInterfacePtr const &iface) {
        auto &ifaces = getInterfacesForPath(iface->getPath());
        bool ret = ifaces.empty();
        ifaces.push_back(iface);
        return ret;
    }

    bool InterfaceTree::removeInterface(ClientInterfacePtr const &iface) {
        auto &ifaces = getInterfacesForPath(iface->getPath());
        auto it = std::find(begin(ifaces), end(ifaces), iface);
        if (it != end(ifaces)) {
            ifaces.erase(it);
        }
        return ifaces.empty();
    }

    InterfaceTree::value_type &
    InterfaceTree::getInterfacesForPath(std::string const &path) {
        return getNodeForPath(path).value();
    }

    InterfaceTree::node_type &
    InterfaceTree::getNodeForPath(std::string const &path) {
        return common::detail::pathParseAndRetrieve(path, *m_root);
    }
} // namespace client
} // namespace osvr