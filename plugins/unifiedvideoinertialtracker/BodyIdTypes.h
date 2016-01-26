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

#ifndef INCLUDED_BodyIdTypes_h_GUID_4FD241A3_25E9_42AF_F075_8EE8191C02F0
#define INCLUDED_BodyIdTypes_h_GUID_4FD241A3_25E9_42AF_F075_8EE8191C02F0

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/TypeSafeId.h>

// Standard includes
#include <stdexcept>
#include <cstdint>

namespace osvr {
namespace vbtracker {
    namespace detail {
        /// Type tag for type-safe body ID
        struct BodyIdTag;
    } // namespace detail
} // namespace vbtracker
namespace util {
    namespace typesafeid_traits {
        /// Tag-based specialization of underlying value type for beacon ID
        template <> struct WrappedType<vbtracker::detail::BodyIdTag> {
            using type = std::uint16_t;
        };
    } // namespace typesafeid_traits
} // namespace util

namespace vbtracker {
    /// Type-safe zero-based beacon ID.
    using BodyId = util::TypeSafeId<detail::BodyIdTag>;
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BodyIdTypes_h_GUID_4FD241A3_25E9_42AF_F075_8EE8191C02F0
