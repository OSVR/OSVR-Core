/** @file
    @brief Header providing an integer type by size.

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

#ifndef INCLUDED_SizedInt_h_GUID_E228EDD2_8659_488A_8D2D_8E9BF81E7A36
#define INCLUDED_SizedInt_h_GUID_E228EDD2_8659_488A_8D2D_8E9BF81E7A36

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cstdint>
#include <cstddef>

namespace osvr {
namespace util {
    namespace detail {
        template <std::size_t bytes, bool isSigned> struct sized_int;
/// Set up specializations for the standard int types.
#define OSVR_INT_TRAITS(BITS)                                                  \
    template <> struct sized_int<sizeof(std::int##BITS##_t), true> {           \
        using type = std::int##BITS##_t;                                       \
    };                                                                         \
    template <> struct sized_int<sizeof(std::uint##BITS##_t), false> {         \
        using type = std::uint##BITS##_t;                                      \
    }
        OSVR_INT_TRAITS(8);
        OSVR_INT_TRAITS(16);
        OSVR_INT_TRAITS(32);
        OSVR_INT_TRAITS(64);
#undef OSVR_INT_TRAITS
        /// Maximum size with a valid specialization.
        static const auto MAX_SIZE = sizeof(std::uint64_t);

        /// The size that has a specialization to stop recursion.
        static const auto ERROR_SPECIALIZATION_SIZE = MAX_SIZE + 1;

        /// Case to catch those who exceed the max size
        template <bool isSigned>
        struct sized_int<ERROR_SPECIALIZATION_SIZE, isSigned> {};
        /// Case for non-exact sizes, incrementing by one byte if we haven't
        /// skipped right over the error case already.
        template <std::size_t bytes, bool isSigned>
        struct sized_int
            : sized_int<(bytes > MAX_SIZE) ? ERROR_SPECIALIZATION_SIZE
                                           : bytes + 1,
                        isSigned> {

            static_assert(bytes <= MAX_SIZE,
                          "No standard integral types larger than [u]int64_t.");
        };

    } // namespace detail
    /// Alias providing an integer type (signed or unsigned, your choice)
    /// containing at least as many bytes as you requested.
    template <std::size_t minBytes, bool isSigned = false>
    using sized_int_t = typename detail::sized_int<minBytes, isSigned>::type;

} // namespace util
} // namespace osvr

#endif // INCLUDED_SizedInt_h_GUID_E228EDD2_8659_488A_8D2D_8E9BF81E7A36
