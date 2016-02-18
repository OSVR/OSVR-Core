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
    /// any --hide-xyz into --xyz false and --show-xyz into --xyz true
    inline std::pair<std::string, std::string>
    convertProgramOptionShowHideIntoTrueFalse(std::string s) {
        static const char HIDE[] = "--hide-";
        static const char SHOW[] = "--show-";
        if (0 == s.find(SHOW)) {
            // argument started with --show-
            // strip it off to get the real option and give it the value "true"
            s.erase(0, sizeof(SHOW) - 1);
            return std::make_pair(s, std::string("true"));
        } else if (0 == s.find(HIDE)) {
            // argument started with --hide-
            // strip it off to get the real option and give it the value "false"
            s.erase(0, sizeof(HIDE) - 1);
            return std::make_pair(s, std::string("false"));
        }
        // program option we weren't able to add value to.
        return std::make_pair(std::string(), std::string());
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_ProgramOptionsToggleFlags_h_GUID_9879F3E3_4C35_4389_010C_FD82D5878172
