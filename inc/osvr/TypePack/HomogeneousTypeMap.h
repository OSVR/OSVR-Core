/** @file
    @brief Header with a run-time extensions to the TypePack simple
    template-parameter-pack-based metaprogramming library

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef INCLUDED_HomogeneousTypeMap_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
#define INCLUDED_HomogeneousTypeMap_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC

// Internal Includes
#include "Contains.h"
#include "FindFirst.h"
#include "Length.h"

// Library/third-party includes
// - none

// Standard includes
#include <array>

namespace osvr {
namespace typepack {
    /// @brief A class that uses types as an index into a container with
    /// uniform-typed contents, somewhat like a map except all elements are
    /// default-constructed rather than having an optional "not set" status.
    /// (You may emulate this by providing a specialization of
    /// boost::optional<> as your value type.)
    template <typename KeyList, typename ValueType> class HomogeneousTypeMap {
      public:
        using value_type = ValueType;
        using reference = value_type &;
        using const_reference = value_type const &;
        using key_types = KeyList;
        template <typename Key> using valid_key = contains<key_types, Key>;
        template <typename Key> using index = find_first<key_types, Key>;

        template <typename Key> reference operator()() {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

        template <typename Key> const_reference operator()() const {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

        template <typename Key> reference operator[](Key &&) {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

        template <typename Key> const_reference operator[](Key &&) const {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

      private:
        static const auto SIZE = length<key_types>::value;
        using container = std::array<value_type, SIZE>;
        container m_contents;
    };
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_TypePackRuntime_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
