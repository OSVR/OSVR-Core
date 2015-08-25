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
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Common/ClientInterface.h>
#include "../Common/PathParseAndRetrieve.h" /// @todo internal header cross-include

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>

namespace osvr {
namespace client {
    class HandlerClearVisitor {
      public:
        HandlerClearVisitor(InterfaceTree &tree) : m_tree(&tree) {}
        void operator()(InterfaceTree::node_type &node) {
            m_tree->m_removeHandler(node);
            node.visitChildren(*this);
        }

      private:
        InterfaceTree *m_tree;
    };

    InterfaceTree::InterfaceTree() : m_root(node_type::createRoot()) {}

    bool InterfaceTree::addInterface(common::ClientInterfacePtr const &iface) {
        auto &ifaces = getInterfacesForPath(iface->getPath());
        bool ret = ifaces.empty();

        // Makes sure we only have one copy of any interface pointer in the
        // vector
        auto it = std::find(begin(ifaces), end(ifaces), iface);
        if (it == end(ifaces)) {
            ifaces.push_back(iface);
        }
        return ret;
    }

    bool
    InterfaceTree::removeInterface(common::ClientInterfacePtr const &iface) {
        auto &ifaces = getInterfacesForPath(iface->getPath());
        auto it = std::find(begin(ifaces), end(ifaces), iface);
        if (it != end(ifaces)) {
            ifaces.erase(it);
        }
        return ifaces.empty();
    }

    common::InterfaceList &
    InterfaceTree::getInterfacesForPath(std::string const &path) {
        return m_getNodeForPath(path).value().interfaces;
    }

    RemoteHandlerPtr InterfaceTree::getHandlerForPath(std::string const &path) {
        return m_getNodeForPath(path).value().handler;
    }

    RemoteHandlerPtr
    InterfaceTree::eraseHandlerForPath(std::string const &path) {
        return m_removeHandler(m_getNodeForPath(path));
    }

    RemoteHandlerPtr
    InterfaceTree::replaceHandlerForPath(std::string const &path,
                                         RemoteHandlerPtr const &handler) {
        return m_setHandler(m_getNodeForPath(path), handler);
    }

    void InterfaceTree::updateHandlers() { m_handlers.update(); }
    void InterfaceTree::clearHandlers() {
        HandlerClearVisitor visitor{*this};
        visitor(*m_root);
    }

    InterfaceTree::node_type &
    InterfaceTree::m_getNodeForPath(std::string const &path) {
        return common::pathParseAndRetrieve(*m_root, path);
    }

    RemoteHandlerPtr InterfaceTree::m_removeHandler(node_type &node) {
        auto ret = node.value().handler;
        node.value().handler.reset();
        m_handlers.remove(ret);
        return ret;
    }

    RemoteHandlerPtr
    InterfaceTree::m_setHandler(node_type &node,
                                RemoteHandlerPtr const &handler) {
        auto ret = m_removeHandler(node);
        node.value().handler = handler;
        m_handlers.add(handler);
        return ret;
    }
} // namespace client
} // namespace osvr