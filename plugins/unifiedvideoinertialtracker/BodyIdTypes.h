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
#include <osvr/Util/TypeSafeIdHash.h>

// Standard includes
#include <stdexcept>
#include <cstdint>
#include <type_traits>
#include <limits.h>

namespace osvr {
namespace vbtracker {
    namespace detail {
        /// Type tag for type-safe body ID
        struct BodyIdTag;
        /// Type tag for type-safe target ID (per body)
        struct TargetIdTag;
    } // namespace detail
} // namespace vbtracker
namespace util {
    namespace typesafeid_traits {
        /// Tag-based specialization of underlying value type for body ID
        template <> struct WrappedType<vbtracker::detail::BodyIdTag> {
            using type = std::uint16_t;
        };
        /// Tag-based specialization of underlying value type for target ID
        template <> struct WrappedType<vbtracker::detail::TargetIdTag> {
            using type = std::uint8_t;
        };
    } // namespace typesafeid_traits
} // namespace util

namespace vbtracker {
    /// Type-safe zero-based body ID.
    using BodyId = util::TypeSafeId<detail::BodyIdTag>;
    /// Type-safe zero-based target ID.
    using TargetId = util::TypeSafeId<detail::TargetIdTag>;
    /// Type-safe zero-based target ID qualified with its body ID.
    using BodyTargetId = std::pair<BodyId, TargetId>;

    /// Stream output operator for the body-target ID.
    template <typename Stream>
    inline Stream &operator<<(Stream &os, BodyTargetId const &id) {
        os << id.first.value() << ":" << int(id.second.value());
        return os;
    }
} // namespace vbtracker
} // namespace osvr

namespace std {
template <> struct hash<osvr::vbtracker::BodyTargetId> {
    using PairType = osvr::vbtracker::BodyTargetId;
    using First = PairType::first_type;
    using Second = PairType::second_type;

    using FirstWrapped = First::wrapped_type;
    using SecondWrapped = Second::wrapped_type;
    using NewType = std::uint32_t;
    static_assert(sizeof(FirstWrapped) + sizeof(SecondWrapped) <=
                      sizeof(NewType),
                  "New type must be chosen to accommodate the combined bits "
                  "from both body and target ID!");

    size_t operator()(const PairType &x) const {
        auto extendedFirst = static_cast<NewType>(x.first.value());
        auto extendedSecond = static_cast<NewType>(x.second.value());
        /// Using xor for safety, in case I got the shifts wrong, but it should
        /// be [padding][bits from first element][bits from second element]
        NewType combinedValue =
            (extendedFirst << (CHAR_BIT * sizeof(SecondWrapped))) ^
            extendedSecond;
        return std::hash<NewType>{}(combinedValue);
    }
};
} // namespace std

#endif // INCLUDED_BodyIdTypes_h_GUID_4FD241A3_25E9_42AF_F075_8EE8191C02F0
