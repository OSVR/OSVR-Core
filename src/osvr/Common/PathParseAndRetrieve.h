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

#ifndef INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C
#define INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C

// Internal Includes
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Common/RoutingExceptions.h>
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
#include <boost/assert.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/adaptor/sliced.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    namespace detail {

        /// @brief Internal method for parsing a path and getting
        /// or creating the nodes along it.
        ///
        /// @param path An absolute path (beginning with /) or a path relative
        /// to the node (no leading /) - a trailing slash is trimmed silently
        /// @param node A node of a tree. If a leading slash is found on the
        /// path, the node will be used to find the root, otherwise it is
        /// considered the place to which the given path is relative.
        /// @param permitParent An optional flag indicating if ".." is permitted
        /// as a component of the path, moving to the parent level.
        ///
        /// If nodes do not exist, they are created as default. An empty path
        /// results in returning the same node provided. A component of a path
        /// equal to "." is effectively ignored ("current directory" behavior)
        ///
        /// @returns a reference to the node referred to by the path.
        /// @throws exceptions::EmptyPathComponent,
        /// exceptions::ForbiddenParentPath, exceptions::ImpossibleParentPath
        template <typename ValueType>
        inline util::TreeNode<ValueType> &
        treePathRetrieve(std::string const &path,
                         util::TreeNode<ValueType> &node,
                         bool permitParent = false) {
            typedef util::TreeNode<ValueType> Node;

            if (path.empty()) {
                return node;
            }
            Node *ret = &node;

            // Check for leading slash, indicating absolute path
            if (path.at(0) == getPathSeparatorCharacter()) {
                // Got it, so move to the root of the tree.
                while (!ret->isRoot()) {
                    ret = ret->getParent();
                }
                if (path == getPathSeparator()) {
                    // Literally just asking for the root.
                    return *ret;
                }
                // Remove the leading slash for the iterator's benefit.
                path.erase(begin(path));
            }

            // Remove any trailing slash
            if (path.back() == getPathSeparatorCharacter()) {
                path.pop_back();
            }

            // new scope for the iterators and loop:
            {
                // Create a boost string algorithm "split iterator" to iterate
                // through components of the path.
                auto begin = boost::algorithm::make_split_iterator(
                    path,
                    boost::first_finder(getPathSeparator(), boost::is_equal()));
                // Create the corresponding end iterator: same type as begin,
                // but default constructed.
                auto end = decltype(begin)();

                // Temporary string that will be re-used each pass through the
                // loop
                std::string component;

                // Use the iterators as a range in a loop, to process each
                // component of the path
                for (auto rangeIt : boost::make_iterator_range(begin, end)) {
                    // Extract the component to a string for interpretation.
                    component = boost::copy_range<std::string>(rangeIt);

                    // Interpret the component: four cases
                    if (component.empty()) {
                        // Empty components are forbidden
                        throw exceptions::EmptyPathComponent(path);
                    } else if (component == ".") {
                        // current location - go to the next component without
                        // changing location
                        continue;
                    } else if (component == "..") {
                        // parent path - must check for permission first, then
                        // possibility (root has no parent)
                        if (!permitParent) {
                            throw exceptions::ForbiddenParentPath();
                        }
                        if (ret->isRoot()) {
                            throw exceptions::ImpossibleParentPath();
                        }
                        ret = ret->getParent();
                    } else {
                        // A non-special string: just get the child
                        ret = &(ret->getOrCreateChildByName(component));
                    }
                    // if we make it to here we've updated ret.
                }
            }

            return *ret;
        }

        /// @brief Internal method for parsing a path and getting or creating
        /// the nodes along it.
        /// @param path An absolute path (beginning with /) - a trailing slash
        /// is trimmed silently
        /// @param root The root node of a tree. This is not checked at runtime
        /// (just a debug assert) since this should only be called from safe,
        /// internal locations!
        ///
        /// If nodes do not exist, they are created as default
        ///
        /// @returns a reference to the leaf node referred to by the path.
        /// @throws exceptions::PathNotAbsolute, exceptions::EmptyPath,
        /// exceptions::EmptyPathComponent, exceptions::ForbiddenParentPath
        template <typename ValueType>
        inline util::TreeNode<ValueType> &
        pathParseAndRetrieve(std::string path,
                             util::TreeNode<ValueType> &root) {
            BOOST_ASSERT_MSG(root.isRoot(), "Must pass the root node!");
            if (path.empty()) {
                throw exceptions::EmptyPath();
            }
            if (path == getPathSeparator()) {
                return root;
            }
            if (path.at(0) != getPathSeparatorCharacter()) {
                throw exceptions::PathNotAbsolute(path);
            }

            return treePathRetrieve(path, root);
        }
    } // namespace detail
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C
