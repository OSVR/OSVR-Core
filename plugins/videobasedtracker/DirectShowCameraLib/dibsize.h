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

#ifndef INCLUDED_dibsize_h_GUID_0D57E05D_AC88_44B5_8EB6_0FBD416AFFDA
#define INCLUDED_dibsize_h_GUID_0D57E05D_AC88_44B5_8EB6_0FBD416AFFDA

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cmath>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/// @brief Computes bytes required for an UNCOMPRESSED RGB DIB
inline DWORD dibsize(BITMAPINFOHEADER const &bi) {
    // cf:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318229(v=vs.85).aspx
    auto stride = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) >> 3);
    return stride * std::abs(bi.biHeight);
}

#endif // INCLUDED_dibsize_h_GUID_0D57E05D_AC88_44B5_8EB6_0FBD416AFFDA
