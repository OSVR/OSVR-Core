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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/PathTree.h>
#include <osvr/Client/ClientInterfaceObjectManager.h>
#include <osvr/Client/RemoteHandlerFactory.h>
#include <osvr/Common/PathTreeObserver.h>
#include <osvr/Common/PathTreeOwner.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/ResolveTreeNode.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <unordered_set>

namespace osvr {
namespace client {
    ClientInterfaceObjectManager::ClientInterfaceObjectManager(
        common::PathTreeOwner &tree, RemoteHandlerFactory &handlerFactory,
        common::ClientContext &ctx)
        : m_pathTree(tree.get()), m_treeObserver(tree.makeObserver()),
          m_factory(handlerFactory), m_ctx(&ctx) {
        m_treeObserver->setEventCallback(
            common::PathTreeEvents::AboutToUpdate,
            [&](common::PathTree &) { m_interfaces.clearHandlers(); });
        m_treeObserver->setEventCallback(
            common::PathTreeEvents::AfterUpdate,
            [&](common::PathTree &) { m_connectNeededCallbacks(); });
    }

    void ClientInterfaceObjectManager::addInterface(
        common::ClientInterfacePtr const &iface) {
        auto pin = iface;
        const auto isNew = m_interfaces.addInterface(pin);
        if (isNew) {
            m_connectCallbacksOnPath(pin->getPath());
        }
    }
    void ClientInterfaceObjectManager::releaseInterface(
        common::ClientInterfacePtr const &iface) {
        auto pin = iface;
        const auto isEmpty = m_interfaces.removeInterface(pin);
        if (isEmpty) {
            m_removeCallbacksOnPath(pin->getPath());
        }
    }

    void ClientInterfaceObjectManager::updateHandlers() {
        m_interfaces.updateHandlers();
    }

    bool ClientInterfaceObjectManager::m_connectCallbacksOnPath(
        std::string const &path) {
        /// Start by removing handler from interface tree and handler container
        /// for this path, if found. Ensures that if we early-out (fail to set
        /// up a handler) we don't have a leftover one still active.
        m_interfaces.eraseHandlerForPath(path);

        auto source = common::resolveTreeNode(m_pathTree, path);
        if (!source.is_initialized()) {
            OSVR_DEV_VERBOSE("Could not resolve source for " << path);
            return false;
        }

        auto handler = m_factory.invokeFactory(
            *source, m_interfaces.getInterfacesForPath(path), *m_ctx);

        if (handler) {
            OSVR_DEV_VERBOSE("Successfully produced handler for " << path);
            // Store the new handler in the interface tree
            auto oldHandler = m_interfaces.replaceHandlerForPath(path, handler);
            BOOST_ASSERT_MSG(
                !oldHandler,
                "We removed the old handler before so it should be null now");
            return true;
        }

        OSVR_DEV_VERBOSE("Could not produce handler for " << path);
        return false;
    }

    /// @brief Given a path, remove any existing handler for that path from
    /// both the handler container and the interface tree.
    void ClientInterfaceObjectManager::m_removeCallbacksOnPath(
        std::string const &path) {
        m_interfaces.eraseHandlerForPath(path);
    }

    void ClientInterfaceObjectManager::m_connectNeededCallbacks() {
        auto failedPaths = std::unordered_set<std::string>{};
        auto successfulPaths = size_t{0};
        /// Call our little lambda with every path that has interfaces but no
        /// handler, and see what we can do.
        m_interfaces.visitPathsWithoutHandlers([&](std::string const &path) {
            auto success = m_connectCallbacksOnPath(path);
            if (success) {
                successfulPaths++;
            } else {
                failedPaths.insert(path);
            }
        });
        OSVR_DEV_VERBOSE("Connected " << successfulPaths << " of "
                                      << successfulPaths + failedPaths.size()
                                      << " unconnected paths successfully");
    }
} // namespace client
} // namespace osvr
