/** @file
    @brief Header with a convenience function to make a std::string out of a
   non-null-terminated char array (string literal) like that produced by
   osvr_json_to_c.

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

#ifndef INCLUDED_StringLiteralFileToString_h_GUID_8B63CB02_19E7_4C9A_40C6_E5AFC056DA2E
#define INCLUDED_StringLiteralFileToString_h_GUID_8B63CB02_19E7_4C9A_40C6_E5AFC056DA2E

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <stddef.h>

namespace osvr {
namespace util {
    /// @brief Safely and easily convert a literal array of characters (like
    /// from osvr_json_to_c) into a std::string.
    template <size_t N>
    inline std::string makeString(const char(&arrayLiteral)[N]) {
        return std::string(arrayLiteral, N);
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_StringLiteralFileToString_h_GUID_8B63CB02_19E7_4C9A_40C6_E5AFC056DA2E
