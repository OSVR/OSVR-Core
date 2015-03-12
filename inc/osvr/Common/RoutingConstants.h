/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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

#ifndef INCLUDED_RoutingConstants_h_GUID_1A7E9AA8_23B3_409B_4589_A15870E9473D
#define INCLUDED_RoutingConstants_h_GUID_1A7E9AA8_23B3_409B_4589_A15870E9473D

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @name Constants
    /// @ingroup Routing
    /// @{
    /// @brief Gets the path separator character - a slash.
    OSVR_COMMON_EXPORT char getPathSeparatorCharacter();

    /// @brief Gets the path separator - a slash - as a null-terminated
    /// string.
    OSVR_COMMON_EXPORT const char *getPathSeparator();
    /// @}
} // namespace common
} // namespace osvr
#endif // INCLUDED_RoutingConstants_h_GUID_1A7E9AA8_23B3_409B_4589_A15870E9473D
