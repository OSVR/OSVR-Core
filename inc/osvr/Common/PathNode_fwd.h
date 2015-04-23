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

#ifndef INCLUDED_PathNode_fwd_h_GUID_3160EFE4_EC7A_4187_5263_ED3303384F7B
#define INCLUDED_PathNode_fwd_h_GUID_3160EFE4_EC7A_4187_5263_ED3303384F7B

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes_fwd.h>
#include <osvr/Util/TreeNode_fwd.h> // IWYU pragma: export

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief The specific tree node type that contains a path element.
    typedef ::osvr::util::TreeNode<PathElement> PathNode;

    /// @brief The ownership pointer of the specific tree node.
    typedef ::osvr::util::TreeNodePointer<PathElement>::type PathNodePtr;

    /// @brief The weak (non-owning) pointer used to refer to a parent tree node
    typedef PathNode *PathNodeParentPtr;

    /// @brief Gets an identifying string for the node value type.
    OSVR_COMMON_EXPORT const char *getTypeName(PathNode const &node);
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathNode_fwd_h_GUID_3160EFE4_EC7A_4187_5263_ED3303384F7B
