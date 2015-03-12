/** @file
    @brief Implementation

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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include "PathParseAndRetrieve.h"
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Common/RoutingExceptions.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/PathElementTools.h>

// Library/third-party includes
#include <boost/assert.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/adaptor/sliced.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    PathNode &pathParseAndRetrieve(std::string path, PathNode &root,
                                   PathElement const &finalComponentDefault) {

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

        PathNode *ret = &root;

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
        auto end =
            decltype(begin)(); // default construct of the same type as begin
        for (auto rangeIt : boost::make_iterator_range(begin, end)) {
            component = boost::copy_range<std::string>(rangeIt);
            OSVR_DEV_VERBOSE(component);
            if (component.empty()) {
                throw exceptions::EmptyPathComponent(path);
            }
            ret = &(ret->getOrCreateChildByName(component));
        }

        // Handle null elements as final component.
        elements::ifNullReplaceWith(ret->value(), finalComponentDefault);
        return *ret;
    }
} // namespace common
} // namespace osvr
