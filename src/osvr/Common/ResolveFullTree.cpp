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
#include <osvr/Common/ResolveFullTree.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Util/TreeTraversalVisitor.h>

// Library/third-party includes
#include <boost/variant.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    inline bool isNodeAnAlias(PathNode const &node) {
        return boost::get<elements::AliasElement>(&node.value()) != nullptr;
    }

    std::vector<std::string> resolveFullTree(PathTree &tree) {
        std::vector<std::string> badPaths;
        osvr::util::traverseWith(
            tree.getRoot(), [&tree, &badPaths](PathNode const &node) {
                auto fullPath = getFullPath(node);
                auto result = resolveTreeNode(tree, fullPath);
                if (!result && isNodeAnAlias(node)) {
                    // OK, so this is an alias (it should have resolved) and yet
                    // it didn't. Add the path to the list of bad paths.
                    badPaths.emplace_back(std::move(fullPath));
                }
            });
        return badPaths;
    }
} // namespace common
} // namespace osvr
