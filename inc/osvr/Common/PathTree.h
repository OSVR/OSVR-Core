/** @file
    @brief Header

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

#ifndef INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071
#define INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071

// Internal Includes
#include <osvr/Common/PathTree_fwd.h> // IWYU pragma: export
#include <osvr/Common/Export.h>
#include <osvr/Common/PathNode_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief A tree representation, with path/url syntax, of the known OSVR
    /// system.
    class PathTree : boost::noncopyable {
      public:
        /// @brief Constructor
        OSVR_COMMON_EXPORT PathTree();

        /// @brief Visitation
        /// @{
        /// @brief Visit the tree, starting at the root, with the given functor.
        template <typename F> void visitTree(F &functor) { functor(*m_root); }

        /// @brief Visit the tree, with const nodes, starting at the root, with
        /// the given functor.
        template <typename F> void visitConstTree(F &functor) const {
            functor(const_cast<PathNode const &>(*m_root));
        }
        /// @}

        /// @brief Returns the node indicated by the path, which must be
        /// absolute (begin with a /). Any non-existent nodes will be created
        /// with values of NullElement
        /// @throws exceptions::PathNotAbsolute, exceptions::EmptyPath,
        /// exceptions::EmptyPathComponent
        OSVR_COMMON_EXPORT PathNode &getNodeByPath(std::string const &path);

        /// @brief Returns the node indicated by the path, which must be
        /// absolute (begin with a /).
        /// @throws exceptions::PathNotAbsolute, exceptions::EmptyPath,
        /// exceptions::EmptyPathComponent, util::tree::NoSuchChild
        OSVR_COMMON_EXPORT PathNode const &
        getNodeByPath(std::string const &path) const;

        /// @overload
        ///
        /// Same as above but a non-existent final component node will be
        /// initialized from the provided element rather than NullElement.
        OSVR_COMMON_EXPORT PathNode &
        getNodeByPath(std::string const &path,
                      PathElement const &finalComponentDefault);

        /// @brief Reset the path tree to a new, empty root node.
        OSVR_COMMON_EXPORT void reset();

        PathNode &getRoot() { return *m_root; }

        PathNode const &getRoot() const { return *m_root; }

      private:
        /// @brief Root node of the tree.
        PathNodePtr m_root;
    };

    /// @brief Make node an alias pointing to source, with the given priority,
    /// if it needs updating.
    ///
    /// @return true if the node was changed
    ///
    /// @relates osvr::common::PathTree
    OSVR_COMMON_EXPORT bool
    addAlias(PathNode &node, std::string const &source,
             AliasPriority priority = ALIASPRIORITY_MANUAL);

    /// @brief Parse an old-style route object (with source and destination),
    /// and use the given node as a entry point into the tree, and add an
    /// aliases based on that route.
    ///
    /// Can also handle normal alias descriptions without complaint.
    ///
    /// @return true if the node was changed
    ///
    /// @relates osvr::common::PathTree
    OSVR_COMMON_EXPORT bool
    addAliasFromRoute(PathNode &node, std::string const &route,
                      AliasPriority priority = ALIASPRIORITY_MANUAL);

    bool addAliasFromSourceAndRelativeDest(
        PathNode &node, std::string const &source, std::string const &dest,
        AliasPriority priority = ALIASPRIORITY_MANUAL);

    bool isPathAbsolute(std::string const &source);

    /// @brief Clones a path tree
    ///
    /// @relates osvr::common::PathTree
    OSVR_COMMON_EXPORT void clonePathTree(PathTree const &src, PathTree &dest);

} // namespace common
} // namespace osvr

#endif // INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071
