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

#ifndef INCLUDED_TypePackRuntime_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
#define INCLUDED_TypePackRuntime_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC

// Internal Includes
#include <osvr/Util/TypePack.h>

// Library/third-party includes
// - none

// Standard includes
#include <array>

namespace osvr {
namespace util {
    /// @brief Run-time extensions to the TypePack simple argument-pack-based
    /// metaprogramming library
    namespace typepack_rt {
        namespace detail {
            template <typename Needle, std::size_t i, typename... Ts>
            struct find_first_impl;
            // Expand lists
            template <typename Needle, typename... Ts>
            struct find_first_impl<Needle, 0, typepack::list<Ts...>>
                : find_first_impl<Needle, 0, Ts...> {};
            // base case: at the head
            template <typename Needle, std::size_t i, typename... Ts>
            struct find_first_impl<Needle, i, Needle, Ts...> {
                using type = typepack::size_t_<i>;
            };
            // Recursive case
            template <typename Needle, std::size_t i, typename Head,
                      typename... Ts>
            struct find_first_impl<Needle, i, Head, Ts...> {
                using type =
                    typepack::t_<find_first_impl<Needle, i + 1, Ts...>>;
            };
            /// base case not found
            template <typename Needle, std::size_t i>
            struct find_first_impl<Needle, i> {};

            template <typename List, typename Needle>
            using find_first = typepack::t_<find_first_impl<Needle, 0, List>>;
        } // namespace detail
        /// @brief A class that uses types as an index into a container with
        /// uniform-typed contents, somewhat like a map except all elements are
        /// default-constructed rather than having an optional "not set" status.
        /// (You may emulate this by providing a specialization of
        /// boost::optional<> as your value type.)
        template <typename KeyList, typename ValueType>
        class HomogeneousTypeMap {
          public:
            using value_type = ValueType;
            using reference = value_type &;
            using const_reference = value_type const &;
            using key_types = KeyList;
            template <typename Key>
            using valid_key = typepack::contains<key_types, Key>;

            template <typename Key> reference operator()() {
                static_assert(valid_key<Key>::value,
                              "Key type not found in the list!");
                return m_contents[detail::find_first<key_types, Key>::value];
            }

            template <typename Key> const_reference operator()() const {
                static_assert(valid_key<Key>::value,
                              "Key type not found in the list!");
                return m_contents[detail::find_first<key_types, Key>::value];
            }

            template <typename Key> reference operator[](Key &&) {
                static_assert(valid_key<Key>::value,
                              "Key type not found in the list!");
                return m_contents[detail::find_first<key_types, Key>::value];
            }

            template <typename Key> const_reference operator[](Key &&) const {
                static_assert(valid_key<Key>::value,
                              "Key type not found in the list!");
                return m_contents[detail::find_first<key_types, Key>::value];
            }

          private:
            static const auto SIZE = typepack::size<key_types>;
            using container = std::array<value_type, SIZE>;
            container m_contents;
        }
    } // namespace typepack
} // namespace util
} // namespace osvr

#endif // INCLUDED_TypePackRuntime_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
