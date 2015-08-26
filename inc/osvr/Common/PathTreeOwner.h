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

#ifndef INCLUDED_PathTreeOwner_h_GUID_3B8C4AD4_90FA_4485_1388_CCFABF5EB66F
#define INCLUDED_PathTreeOwner_h_GUID_3B8C4AD4_90FA_4485_1388_CCFABF5EB66F

// Internal Includes
#include <osvr/Common/PathTreeObserverPtr.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <json/value.h>

// Standard includes
#include <vector>

namespace osvr {
namespace common {
    /// @brief Object responsible for owning a path tree (specifically a
    /// "downstream"/client path tree), replacing its contents from
    /// JSON-serialized data, and notifying a collection of observers of such
    /// events.
    ///
    /// @sa osvr::common::PathTreeObserver
    class PathTreeOwner : private boost::noncopyable {
      public:
        PathTreeOwner() = default;

        /// non-copyable
        PathTreeOwner(PathTreeOwner const &) = delete;
        /// non-copy-assignable
        PathTreeOwner &operator=(PathTreeOwner const &) = delete;

        /// @brief reports whether the path tree has been populated (by a call
        /// to replaceTree() )
        explicit operator bool() const { return m_valid; }

        /// @brief Make an observer object that can hold callbacks for tree
        /// updates. Callbacks are called in the order their observers were
        /// created.
        ///
        /// @sa osvr::common::PathTreeObserver
        OSVR_COMMON_EXPORT PathTreeObserverPtr makeObserver();

        /// @brief Replace the entirety of the path tree from the given
        /// serialized array of nodes.
        OSVR_COMMON_EXPORT void replaceTree(Json::Value const &nodes);

        /// @brief Access the path tree object itself
        PathTree &get() { return m_tree; }

        /// @brief Access the path tree object itself - constant reference
        PathTree const &get() const { return m_tree; }

      private:
        PathTree m_tree;
        std::vector<PathTreeObserverWeakPtr> m_observers;
        bool m_valid = false;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_PathTreeOwner_h_GUID_3B8C4AD4_90FA_4485_1388_CCFABF5EB66F
