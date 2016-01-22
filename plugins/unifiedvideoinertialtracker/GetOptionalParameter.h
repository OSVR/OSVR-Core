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

#ifndef INCLUDED_GetOptionalParameter_h_GUID_F4FA55E0_C946_4AAE_6741_57C269C56D24
#define INCLUDED_GetOptionalParameter_h_GUID_F4FA55E0_C946_4AAE_6741_57C269C56D24

// Internal Includes
// - none

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    namespace detail {
        template <typename T> struct JsonTypeGetter;
#define OSVR_DECLARE_JSON_TYPE_GETTER(TYPENAME, METHOD)                        \
    template <> struct JsonTypeGetter<TYPENAME> {                              \
        static TYPENAME apply(Json::Value const &val) { return val.METHOD(); } \
    };
        OSVR_DECLARE_JSON_TYPE_GETTER(bool, asBool)
        OSVR_DECLARE_JSON_TYPE_GETTER(float, asFloat)
        OSVR_DECLARE_JSON_TYPE_GETTER(double, asDouble)
        OSVR_DECLARE_JSON_TYPE_GETTER(int, asInt)
        OSVR_DECLARE_JSON_TYPE_GETTER(std::string, asString)

#undef OSVR_DECLARE_JSON_TYPE_GETTER
    } // namespace detail
    template <typename T> inline T json_cast(Json::Value const &val) {
        return detail::JsonTypeGetter<T>::apply(val);
    }

    /// Gets an optional parameter from a JSON object: if it's not present, the
    /// existing value is left there.
    template <typename T>
    inline void getOptionalParameter(T &dest, Json::Value const &obj,
                                     const char *key) {
        dest = json_cast<T>(obj.get(key, dest));
    }
    /// Gets an optional array parameter from a JSON object: if it's not
    /// present and the exact size, the existing value is left there.
    template <typename T, Json::Value::ArrayIndex N>
    inline void getOptionalParameter(T (&dest)[N], Json::Value const &obj,
                                     const char *key) {
        Json::Value const &node = obj[key];
        if (!node.isArray()) {
            return;
        }
        if (node.size() != N) {
            return;
        }
        for (Json::Value::ArrayIndex i = 0; i < N; ++i) {
            dest[i] = json_cast<T>(node[i]);
        }
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_GetOptionalParameter_h_GUID_F4FA55E0_C946_4AAE_6741_57C269C56D24
