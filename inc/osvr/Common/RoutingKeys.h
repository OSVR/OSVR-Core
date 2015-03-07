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

#ifndef INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3
#define INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace routing_keys {
        /// @brief The key in a routing directive to access the downstream
        /// destination (routing target) of the data.
        OSVR_COMMON_EXPORT const char *destination();

        /// @brief The key in a routing directive to access the upstream source
        /// of the data
        OSVR_COMMON_EXPORT const char *source();

        /// @brief the key to access a child transform
        OSVR_COMMON_EXPORT const char *child();
    } // namespace routing_keys
} // namespace common
} // namespace osvr
#endif // INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3
