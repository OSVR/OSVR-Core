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

#ifndef INCLUDED_JSONTimestamp_h_GUID_F8E6C4C6_5A2F_4DA3_E633_68824AD1ABAF
#define INCLUDED_JSONTimestamp_h_GUID_F8E6C4C6_5A2F_4DA3_E633_68824AD1ABAF

// Internal Includes
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    static const char SECONDS_KEY[] = "seconds";
    static const char USECONDS_KEY[] = "microseconds";
    inline Json::Value toJson(OSVR_TimeValue const &ts) {
        Json::Value val = Json::objectValue;
        val[SECONDS_KEY] = Json::Value(Json::Int64(ts.seconds));
        val[USECONDS_KEY] = ts.microseconds;
        return val;
    }
    inline OSVR_TimeValue timevalueFromJson(Json::Value const &val) {
        OSVR_TimeValue ts;
        ts.seconds = val[SECONDS_KEY].asInt64();
        ts.microseconds = val[USECONDS_KEY].asInt();
        return ts;
    }

} // namespace common
} // namespace osvr

#endif // INCLUDED_JSONTimestamp_h_GUID_F8E6C4C6_5A2F_4DA3_E633_68824AD1ABAF
