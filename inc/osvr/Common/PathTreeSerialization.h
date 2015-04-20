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

#ifndef INCLUDED_PathTreeSerialization_h_GUID_06DB59AB_C47B_4EA0_253A_7D5A45E94F08
#define INCLUDED_PathTreeSerialization_h_GUID_06DB59AB_C47B_4EA0_253A_7D5A45E94F08

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathTree_fwd.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    /// @brief Serialize a path node to a JSON object
    OSVR_COMMON_EXPORT Json::Value pathNodeToJson(PathNode const &node);

    /// @brief Serialize a path tree to a JSON array of objects, one for each
    /// node
    OSVR_COMMON_EXPORT Json::Value pathTreeToJson(PathTree const &tree,
                                                  bool keepNulls = false);

    /// @brief Deserialize a path tree from a JSON array of objects
    OSVR_COMMON_EXPORT void jsonToPathTree(PathTree &tree, Json::Value nodes);
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathTreeSerialization_h_GUID_06DB59AB_C47B_4EA0_253A_7D5A45E94F08
