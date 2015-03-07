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

#ifndef INCLUDED_WrapRoute_h_GUID_CC93B3E1_2C30_42AE_9880_FE43AD40AB81
#define INCLUDED_WrapRoute_h_GUID_CC93B3E1_2C30_42AE_9880_FE43AD40AB81

// Internal Includes
#include <osvr/Common/RoutingKeys.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

inline Json::Value wrapRoute(Json::Value currentRoute, Json::Value newLevel) {
    namespace routing_keys = osvr::common::routing_keys;
    Json::Value ret = currentRoute;
    Json::Value prevSource = currentRoute[routing_keys::source()];
    ret[routing_keys::source()] = newLevel;
    ret[routing_keys::source()][routing_keys::child()] = prevSource;
    return ret;
}

#endif // INCLUDED_WrapRoute_h_GUID_CC93B3E1_2C30_42AE_9880_FE43AD40AB81
