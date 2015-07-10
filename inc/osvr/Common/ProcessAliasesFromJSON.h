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

#ifndef INCLUDED_ProcessAliasesFromJSON_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9
#define INCLUDED_ProcessAliasesFromJSON_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes_fwd.h>
#include <osvr/Common/PathNode_fwd.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    /// @brief Options struct for processAliasesFromJSON() that can be used with
    /// the "chained methods" idiom.
    struct PathProcessOptions {
        bool permitRelativePath = false;
        bool permitRelativeSource = false;
        AliasPriority defaultPriority = ALIASPRIORITY_AUTOMATIC;

        /// @brief Turn on permitRelativePath in a chained method.
        PathProcessOptions &enableRelativePath() {
            permitRelativePath = true;
            return *this;
        }

        /// @brief Turn on permitRelativeSource in a chained method.
        PathProcessOptions &enableRelativeSource() {
            permitRelativeSource = true;
            return *this;
        }

        /// @brief Set defaultPriority in a chained method.
        PathProcessOptions &setDefaultPriority(AliasPriority prio) {
            defaultPriority = prio;
            return *this;
        }
    };

    /// @brief Adds the given aliases to the tree.
    /// @return true if changes were made
    OSVR_COMMON_EXPORT bool processAliasesFromJSON(PathNode &node,
                                                   Json::Value const &val,
                                                   PathProcessOptions opts);

} // namespace common
} // namespace osvr
#endif // INCLUDED_ProcessAliasesFromJSON_h_GUID_9C10FB5E_F8B9_44BB_BCF4_9E50FFBFA1C9
