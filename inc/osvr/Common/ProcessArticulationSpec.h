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

#ifndef INCLUDED_ProcessArticulationSpec_h_GUID_5B956FB5_C8C1_48AF_473A_7146EF0399B0
#define INCLUDED_ProcessArticulationSpec_h_GUID_5B956FB5_C8C1_48AF_473A_7146EF0399B0

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathElementTypes_fwd.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <json/value.h>

namespace osvr {
namespace common {
    /// @brief Set up a path tree based on articulationSpec
    ///
    /// @return true if changes were made
    void processArticulationSpec(PathNode &devNode, Json::Value const &desc);

    OSVR_COMMON_EXPORT void
    processArticulationSpecForPathTree(PathTree &tree,
                                       std::string const &deviceName,
                                       Json::Value const &articSpec);
} // namespace common
} // namespace osvr

#endif // INCLUDED_ProcessArticulationSpec_h_GUID_5B956FB5_C8C1_48AF_473A_7146EF0399B0
