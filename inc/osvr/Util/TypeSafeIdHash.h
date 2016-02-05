/** @file
    @brief Header defining specializations of std::hash that forward to the
   contained integer type, primarily to allow TypeSafeId values to be used in as
   keys in unordered_map<> and friends. Also contains on-request specializations
   for use with aggregates of type-safe IDs.

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

#ifndef INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0
#define INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0

// Internal Includes
#include <osvr/Util/TypeSafeId.h>
#include <osvr/Util/SizedInt.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <type_traits>
#include <cassert>
#include <climits>

namespace std {
template <typename Tag> struct hash<osvr::util::TypeSafeId<Tag>> {
    using TypeSafeIdKeyType = osvr::util::TypeSafeId<Tag>;
    using WrappedType = typename TypeSafeIdKeyType::wrapped_type;
    size_t operator()(const TypeSafeIdKeyType &x) const {
        return std::hash<WrappedType>{}(x.value());
    }
};
} // namespace std

namespace osvr {
namespace util {
    namespace detail {
        template <typename NewType> class IntegerComposition {
          public:
            template <typename T> void operator+=(T newVal) {
                auto extendedNewVal = static_cast<NewType>(newVal);
                assert(
                    !full() &&
                    "Can't compose another integer when we're already full!");
                if (0 == m_bytes) {
                    // first value
                    m_val = extendedNewVal;
                } else {
                    // not the first - shift then xor (for safety in case the
                    // shifts are goofed up.)
                    m_val = (m_val << (CHAR_BIT * sizeof(T))) ^ extendedNewVal;
                }
                m_bytes += sizeof(T);

                assert(!overfull() && "Composition resulted in losing high "
                                      "order bits - we're overfull!");
            }
            NewType get() const { return m_val; }
            bool full() const { return m_bytes >= sizeof(NewType); }
            bool overfull() const { return m_bytes > sizeof(NewType); }

          private:
            NewType m_val = 0;
            std::size_t m_bytes = 0;
        };
    } // namespace detail
    /// Template specialized for common aggregates of TypeSafeIds, with the
    /// intent that you can simply open `namespace std` and derive your
    /// specialization of `struct hash` from this.
    /// e.g.
    /// <code>
    /// namespace std {
    ///     template <>
    ///     struct hash<osvr::vbtracker::BodyTargetId> :
    ///         osvr::util::HashIdAggregate<osvr::vbtracker::BodyTargetId>
    ///         {};
    /// } // namespace std
    /// </code>
    template <typename IdAggregate> struct HashIdAggregate;

    template <typename FirstTag, typename SecondTag>
    struct HashIdAggregate<
        std::pair<TypeSafeId<FirstTag>, TypeSafeId<SecondTag>>> {
        using PairType = std::pair<TypeSafeId<FirstTag>, TypeSafeId<SecondTag>>;

        using First = TypeSafeId<FirstTag>; // PairType::first_type;
        static_assert(std::is_same<First, typename PairType::first_type>::value,
                      "Internal consistency error");

        using Second = TypeSafeId<SecondTag>; // PairType::second_type;
        static_assert(
            std::is_same<Second, typename PairType::second_type>::value,
            "Internal consistency error");

        using FirstWrapped = typename First::wrapped_type;
        using SecondWrapped = typename Second::wrapped_type;
        using NewType =
            sized_int_t<sizeof(FirstWrapped) + sizeof(SecondWrapped)>;
        static_assert(
            sizeof(FirstWrapped) + sizeof(SecondWrapped) <= sizeof(NewType),
            "New type cannot accommodate the combined bits from both IDs!");

        size_t operator()(const PairType &x) const {
            /// Using xor for safety internally, in case I got the shifts wrong,
            /// but it should be:
            /// [padding][bits from first element][bits from second element]
            auto compose = detail::IntegerComposition<NewType>{};
            compose += x.first.value();
            compose += x.second.value();
            return std::hash<NewType>{}(compose.get());
        }
    };

} // namespace util
} // namespace osvr

#endif // INCLUDED_TypeSafeIdHash_h_GUID_1F2936E7_99CB_4D82_9B8D_1D994E2E17F0
