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

#ifndef INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
#define INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    namespace tree {
        template <typename ValueType> class TreeNode;

        /// @brief Dummy struct containing the ownership pointer type for a
        /// TreeNode. (Actually a metafunction...)
        template <typename ValueType> struct TreeNodePointer {
            /// @brief Tree node pointer type (metafunction result type)
            typedef shared_ptr<TreeNode<ValueType> > type;
        };
    } // namespace tree
    using tree::TreeNode;
    using tree::TreeNodePointer;
} // namespace util
} // namespace osvr

#endif // INCLUDED_TreeNode_fwd_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
