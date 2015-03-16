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
        /// @brief Internal method for parsing a path and getting or creating
        /// the
        /// nodes along it.
        /// @param path An absolute path (beginning with /) - a trailing slash
        /// is
        /// trimmed silently
        /// @param root The root node of a tree. This is not checked at runtime
        /// (just a debug assert) since this should only be called from safe,
        /// internal locations!
        ///
        /// If nodes do not exist, they are created as default
        ///
        /// @returns a reference to the leaf node referred to by the path.
        /// @throws exceptions::PathNotAbsolute, exceptions::EmptyPath,
        /// exceptions::EmptyPathComponent
        template <typename ValueType>
        inline util::TreeNode<ValueType> &
        pathParseAndRetrieve(std::string path,
                             util::TreeNode<ValueType> &root) {
            typedef util::TreeNode<ValueType> Node;
            using boost::algorithm::make_split_iterator;
            using boost::first_finder;
            using boost::is_equal;
            BOOST_ASSERT_MSG(root.isRoot(), "Must pass the root node!");
            if (path.empty()) {
                throw exceptions::EmptyPath();
            }
            if (path == getPathSeparator()) {
                /// @todo is an empty path valid input?
                return root;
            }
            if (path.at(0) != getPathSeparatorCharacter()) {
                throw exceptions::PathNotAbsolute(path);
            }

            Node *ret = &root;

            // Determine if there's a trailing slash and remove it.
            if (path.back() == getPathSeparatorCharacter()) {
                path.pop_back();
            }

            // Remove the leading slash for the iterator's benefit.
            path.erase(begin(path));

            // Iterate through the chunks of the path, split by a slash.
            std::string component;
            auto begin = make_split_iterator(
                path, first_finder(getPathSeparator(), is_equal()));
            auto end = decltype(
                begin)(); // default construct of the same type as begin
            for (auto rangeIt : boost::make_iterator_range(begin, end)) {
                component = boost::copy_range<std::string>(rangeIt);
                if (component.empty()) {
                    throw exceptions::EmptyPathComponent(path);
                }
                ret = &(ret->getOrCreateChildByName(component));
            }

            return *ret;
        }
    } // namespace detail
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C
