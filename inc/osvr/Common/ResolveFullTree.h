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

#ifndef INCLUDED_ResolveFullTree_h_GUID_FBD87E7F_9F4B_43A3_4628_9F3E7D0187D7
#define INCLUDED_ResolveFullTree_h_GUID_FBD87E7F_9F4B_43A3_4628_9F3E7D0187D7

// Internal Includes
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Traverse the given path tree, resolving all aliases found to
    /// fully populate any generated sensor targets.
    OSVR_COMMON_EXPORT void resolveFullTree(PathTree &tree);
} // namespace common
} // namespace osvr

#endif // INCLUDED_ResolveFullTree_h_GUID_FBD87E7F_9F4B_43A3_4628_9F3E7D0187D7
