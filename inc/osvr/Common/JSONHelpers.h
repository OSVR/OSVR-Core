/** @file
    @brief Header containing wrappers for some common jsoncpp operations.

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

#ifndef INCLUDED_JSONHelpers_h_GUID_6DB9F077_A09D_44AB_C180_AABFE369637B
#define INCLUDED_JSONHelpers_h_GUID_6DB9F077_A09D_44AB_C180_AABFE369637B

// Internal Includes
// - none

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Parses a string as JSON, returning a null value if parsing fails.
    inline Json::Value jsonParse(std::string const &str) {
        Json::Reader reader;
        Json::Value val = Json::nullValue;
        if (!reader.parse(str, val)) {
            // in case the failed parse modified the value somehow
            val = Json::nullValue;
        }
        return val;
    }

    /// @brief Turns the JSON value into a compact string representation.
    inline std::string jsonToCompactString(Json::Value const& val) {
        Json::FastWriter writer;
        return writer.write(val);
    }

    /// @brief Turns the JSON value into a pretty-printed, human-targeted string
    /// representation.
    inline std::string jsonToStyledString(Json::Value const& val) {
        return val.toStyledString();
    }

} // namespace common
} // namespace osvr

#endif // INCLUDED_JSONHelpers_h_GUID_6DB9F077_A09D_44AB_C180_AABFE369637B
