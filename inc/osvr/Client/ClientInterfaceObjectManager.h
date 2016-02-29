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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ClientInterfaceObjectManager_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004
#define INCLUDED_ClientInterfaceObjectManager_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Common/PathTreeObserverPtr.h>
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/ClientContext_fwd.h>
#include <osvr/Client/InterfaceTree.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace common {
    class PathTreeOwner;
} // namespace common
namespace client {
    class RemoteHandlerFactory;

    /// @brief Class hiding the details of the interaction of the path tree and
    /// interface objects/osvr::client::InterfaceTree.
    ///
    /// Designed to pull out a chunk of functionality from individual
    /// ClientContext implementations for sharing.
    ///
    /// I tend to hate classes named "...Manager", but it seemed like the most
    /// suitable name at the time. Refactor to rename is welcome.
    class ClientInterfaceObjectManager {
      public:
        OSVR_CLIENT_EXPORT
        ClientInterfaceObjectManager(common::PathTreeOwner &tree,
                                     RemoteHandlerFactory &handlerFactory,
                                     common::ClientContext &ctx);
        ClientInterfaceObjectManager &
        operator=(ClientInterfaceObjectManager const &) = delete;

        /// @param verboseFailure Whether we should allow verbose printing of
        /// messages in case this interface doesn't immediately resolve to a
        /// source. For instance, you'd want this to be "false" when setting up
        /// an analysis plugin context ahead of its autodetected sources.
        OSVR_CLIENT_EXPORT void
        addInterface(common::ClientInterfacePtr const &iface,
                     bool verboseFailure = true);
        OSVR_CLIENT_EXPORT void
        releaseInterface(common::ClientInterfacePtr const &iface);

        /// @brief run update on all remote handlers
        OSVR_CLIENT_EXPORT void updateHandlers();

      private:
        /// @brief Given a path, remove any existing handler for that path, then
        /// attempt to fully resolve the path to its source and construct a
        /// handler for it.
        ///
        /// @return true if we were able to connect the path.
        bool m_connectCallbacksOnPath(std::string const &path,
                                      bool verboseFailure = true);

        /// @brief Given a path, remove any existing handler for that path from
        /// both the handler container and the interface tree.
        void m_removeCallbacksOnPath(std::string const &path);

        /// @brief Calls m_connectCallbacksOnPath() for every path that has one
        /// or more interface objects but no remote handler.
        void m_connectNeededCallbacks();

        /// @brief Tree parallel to path tree for holding interface objects and
        /// remote handlers.
        InterfaceTree m_interfaces;

        /// @brief Reference to the main path tree object, retrieved from the
        /// common::PathTreeOwner passed into constructor.
        common::PathTree &m_pathTree;

        /// @brief Path tree "observer" through which we register callbacks on
        /// common::PathTreeOwner events.
        common::PathTreeObserverPtr m_treeObserver;

        /// @brief Factory for producing remote handlers
        RemoteHandlerFactory &m_factory;

        /// @brief The client context that owns us.
        common::ClientContext *m_ctx;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_ClientInterfaceObjectManager_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004
