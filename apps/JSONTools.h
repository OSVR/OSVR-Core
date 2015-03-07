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

#ifndef INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151
#define INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

/// @todo replace this with something in <osvr/Common/JSONEigen.h>
inline Json::Value toJson(Eigen::Vector3d const &vec) {
    Json::Value ret(Json::arrayValue);
    ret.append(vec[0]);
    ret.append(vec[1]);
    ret.append(vec[2]);
    return ret;
}

#endif // INCLUDED_JSONTools_h_GUID_9AB026D8_AE73_47BC_1959_278E34C9B151
