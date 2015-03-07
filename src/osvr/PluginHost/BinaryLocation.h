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

#ifndef INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB
#define INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace pluginhost {
    /// Get this binary's location, as a step in finding the "root" and search
    /// paths.
    std::string getBinaryLocation();
} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB
