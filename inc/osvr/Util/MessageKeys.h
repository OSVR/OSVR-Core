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

#ifndef INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB
#define INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB

// Internal Includes
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Contains methods to retrieve the constant strings identifying
    /// message or sender types.
    namespace messagekeys {
        /// @brief The sender type for system messages.
        OSVR_UTIL_EXPORT const char *systemSender();
        /// @brief Return the string identifying routing data messages
        OSVR_UTIL_EXPORT const char *routingData();
    } // namespace messagekeys
} // namespace util
} // namespace osvr

#endif // INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB
