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

#ifndef INCLUDED_Clamp_h_GUID_E52D654C_917A_4587_39AF_38F98B352C51
#define INCLUDED_Clamp_h_GUID_E52D654C_917A_4587_39AF_38F98B352C51

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>

namespace osvr {
namespace vbtracker {
    /// Returns the value of the parameter v, or of the bounds minVal or maxVal
    /// if it exceeds them.
    template <typename T> inline T clamp(T v, T minVal, T maxVal) {
        return std::max(minVal, std::min(maxVal, v));
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Clamp_h_GUID_E52D654C_917A_4587_39AF_38F98B352C51
