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

#ifndef INCLUDED_InterfaceTree_h_GUID_7F88FD57_3657_4AA6_B725_A1C69AA21ED2
#define INCLUDED_InterfaceTree_h_GUID_7F88FD57_3657_4AA6_B725_A1C69AA21ED2

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Common/InterfaceList.h>
#include <osvr/Util/TreeNode.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Client/RemoteHandler.h>
#include <osvr/Client/HandlerContainer.h>
#include <osvr/Util/TreeNodeFullPath.h>
#include <osvr/Common/RoutingConstants.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace client {

    struct InterfaceTreeValue {
        RemoteHandlerPtr handler;
        common::InterfaceList interfaces;
    };
    /// @brief Holds on to lists of interfaces organized into the tree
    /// structure, as well as their associated handlers.
    class InterfaceTree {
      public:
        typedef InterfaceTreeValue value_type;
        typedef util::TreeNode<value_type> node_type;
        /// @brief Constructor
        OSVR_CLIENT_EXPORT InterfaceTree();

        /// @brief Add an interface to the tree.
        ///
        /// @returns true if this is the first interface for that path.
        OSVR_CLIENT_EXPORT bool
        addInterface(common::ClientInterfacePtr const &iface);

        /// @brief Remove an interface from the tree.
        ///
        /// @returns true if that path no longer has any interfaces.
        OSVR_CLIENT_EXPORT bool
        removeInterface(common::ClientInterfacePtr const &iface);

        /// @brief Returns a reference to the list of interfaces registered for
        /// a given path.
        OSVR_CLIENT_EXPORT common::InterfaceList &
        getInterfacesForPath(std::string const &path);

        /// @brief Returns the handler for a given path.
        OSVR_CLIENT_EXPORT RemoteHandlerPtr
        getHandlerForPath(std::string const &path);

        /// @brief Clears and returns the handler for a given path.
        OSVR_CLIENT_EXPORT RemoteHandlerPtr
        eraseHandlerForPath(std::string const &path);

        /// @brief Sets the handler for a given path, returning the old handler
        /// if any.
        OSVR_CLIENT_EXPORT RemoteHandlerPtr replaceHandlerForPath(
            std::string const &path, RemoteHandlerPtr const &handler);

        /// @brief Call the update method on all handlers.
        OSVR_CLIENT_EXPORT void updateHandlers();

        /// @brief Removes all handlers
        OSVR_CLIENT_EXPORT void clearHandlers();

        /// @brief Visit all paths with interfaces in their list but no handler.
        template <typename F> void visitPathsWithoutHandlers(F &&func) {
            osvr::util::traverseWith(*m_root, [&](node_type &node) {
                if (!node.value().handler && !node.value().interfaces.empty()) {
                    func(util::getTreeNodeFullPath(node,
                                                   common::getPathSeparator()));
                }
            });
        }

      private:
        /// @brief Returns a reference to a node for a given path.
        node_type &m_getNodeForPath(std::string const &path);

        /// @brief Internal helper for removal of a handler
        RemoteHandlerPtr m_removeHandler(node_type &node);

        /// @brief Internal helper for setting of a handler
        RemoteHandlerPtr m_setHandler(node_type &node,
                                      RemoteHandlerPtr const &handler);
        node_type::ptr_type m_root;
        HandlerContainer m_handlers;
        friend class HandlerClearVisitor;
    };

} // namespace client
} // namespace osvr
#endif // INCLUDED_InterfaceTree_h_GUID_7F88FD57_3657_4AA6_B725_A1C69AA21ED2
