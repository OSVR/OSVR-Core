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

#ifndef INCLUDED_ProgramOptionsToggleFlags_h_GUID_9879F3E3_4C35_4389_010C_FD82D5878172
#define INCLUDED_ProgramOptionsToggleFlags_h_GUID_9879F3E3_4C35_4389_010C_FD82D5878172

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <utility>

namespace osvr {
namespace util {
    /// @brief An "additional parser" for Boost.Program_options that will turn
    /// any --hide-xyz into --show-xyz false
    inline std::pair<std::string, std::string>
    convertHideIntoFalseShow(const std::string &s) {
        static const char hidePrefix[] = "--hide";
        static const char showPrefix[] = "show";
        if (s.find(hidePrefix) == 0) {
            return std::make_pair(showPrefix + s.substr(sizeof(hidePrefix) - 1),
                                  std::string("false"));
        }
        return std::make_pair(std::string(), std::string());
    }
}
} // namespace osvr

#endif // INCLUDED_ProgramOptionsToggleFlags_h_GUID_9879F3E3_4C35_4389_010C_FD82D5878172
