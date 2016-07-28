/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_LogUtils_h_GUID_02DD3C28_81BA_41B4_8F4B_37C35A97F361
#define INCLUDED_LogUtils_h_GUID_02DD3C28_81BA_41B4_8F4B_37C35A97F361

// Internal Includes
// - none

// Library/third-party includes
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim_all.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace util {
    namespace log {
        inline std::string sanitizeFilenamePiece(std::string input) {
            namespace alg = boost::algorithm;
            auto acceptable_chars_pred =
                alg::is_alnum() || alg::is_digit() || alg::is_any_of("-_");
            auto virtual_spaces = !acceptable_chars_pred;
            /// This removes leading and trailing bad characters (anything
            /// that's not in the good characters list above) and reduces
            /// internal clumps of them to a single character replaced with _
            alg::trim_fill_if(input, "_", virtual_spaces);
            return input;
        }
    } // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_LogUtils_h_GUID_02DD3C28_81BA_41B4_8F4B_37C35A97F361
