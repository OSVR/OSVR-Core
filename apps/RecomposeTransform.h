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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_RecomposeTransform_h_GUID_4CA04427_C4DC_4898_8938_D735D4A6C056
#define INCLUDED_RecomposeTransform_h_GUID_4CA04427_C4DC_4898_8938_D735D4A6C056

// Internal Includes
#include <osvr/Common/RoutingKeys.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <vector>

/// @brief Goes through your transform, starting at the outermost layer, and
/// keeping and re-composing only the layers that your predicate returns false
/// on.
template <typename Predicate>
inline Json::Value remove_levels_if(Json::Value input, Predicate p) {
    namespace keys = osvr::common::routing_keys;
    std::vector<Json::Value> levels;
    Json::Value current = input;
    while (current.isMember(keys::child()) &&
           current[keys::child()].isObject()) {
        if (!p(current)) {
            levels.push_back(current);
        }
        current = current[keys::child()];
    }
    while (levels.size() > 0) {
        Json::Value next = levels.back();
        levels.pop_back();
        next[keys::child()] = current;
        current = next;
    }
    return current;
}

#endif // INCLUDED_RecomposeTransform_h_GUID_4CA04427_C4DC_4898_8938_D735D4A6C056
