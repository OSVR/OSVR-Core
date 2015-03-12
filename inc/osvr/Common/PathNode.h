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

#ifndef INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0
#define INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathElementTypes.h> ///< @todo can we split out this include? I don't think all consumers of this header need it.
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Gets the absolute path for the given node.
    ///
    /// @ingroup Routing
    OSVR_COMMON_EXPORT std::string getFullPath(PathNode const &node);
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0
