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

#ifndef INCLUDED_GetJSONStringFromTree_h_GUID_031FFB6A_B837_4080_4784_53730E86669E
#define INCLUDED_GetJSONStringFromTree_h_GUID_031FFB6A_B837_4080_4784_53730E86669E

// Internal Includes
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTree.h>

// Library/third-party includes
#include <boost/variant.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {
        struct GetStringVisitor : boost::static_visitor<std::string> {
            std::string operator()(elements::StringElement const &elt) const {
                return elt.getString();
            }
            template <typename T> std::string operator()(T const &) const {
                return std::string{};
            }
        };
    }
    inline std::string getJSONStringAtNode(PathNode const &node) {
        return boost::apply_visitor(GetStringVisitor(), node.value());
    }
    inline std::string getJSONStringFromTree(PathTree const &tree,
                                             std::string const &path) {
        try {
            auto const &desiredNode = tree.getNodeByPath(path);
            return getJSONStringAtNode(desiredNode);
        } catch (util::tree::NoSuchChild &) {
            return std::string{};
        }
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_GetJSONStringFromTree_h_GUID_031FFB6A_B837_4080_4784_53730E86669E
