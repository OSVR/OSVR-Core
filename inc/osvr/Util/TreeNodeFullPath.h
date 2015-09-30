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

#ifndef INCLUDED_TreeNodeFullPath_h_GUID_46FC8EF9_1977_4086_CC13_D85136A55414
#define INCLUDED_TreeNodeFullPath_h_GUID_46FC8EF9_1977_4086_CC13_D85136A55414

// Internal Includes
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
// - none

// Standard includes
#include <sstream>

namespace osvr {
namespace util {
    namespace tree {
        template <typename T>
        inline void buildPathRecursively(util::TreeNode<T> const &node,
                                         const char pathSeparator[],
                                         std::ostream &os) {
            auto parent = node.getParent();
            if (parent) {
                buildPathRecursively(*parent, pathSeparator, os);
            }
            if (!node.isRoot()) {
                os << pathSeparator << node.getName();
            }
        }

        /// @brief Given a tree node and a path separator, get the full path
        /// identifying that tree node.
        template <typename T>
        inline std::string getTreeNodeFullPath(util::TreeNode<T> const &node,
                                               const char pathSeparator[]) {
            /// Special case the root
            if (node.isRoot()) {
                return std::string(pathSeparator);
            }
            std::ostringstream os;
            buildPathRecursively(node, pathSeparator, os);
            return os.str();
        }
    } // namespace tree
    using tree::getTreeNodeFullPath;
} // namespace util
} // namespace osvr

#endif // INCLUDED_TreeNodeFullPath_h_GUID_46FC8EF9_1977_4086_CC13_D85136A55414
