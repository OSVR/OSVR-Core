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

#ifndef INCLUDED_TypeKeyedMap_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
#define INCLUDED_TypeKeyedMap_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC

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
    ///
    /// Values can be accessed with a templated getElement member function, a
    /// subscript operator overload, as well as the nonmember get() free
    /// function templates.
    ///
    /// Runtime performance of element access is constant (equal to array
    /// element access with a constant index)
    template <typename KeyList, typename ValueType> class TypeKeyedMap {
      public:
        using value_type = ValueType;
        using reference = value_type &;
        using const_reference = value_type const &;
        using key_types = KeyList;
        template <typename Key> using valid_key = contains<key_types, Key>;
        template <typename Key> using index = find_first<key_types, Key>;

        /// @brief Accessor by key type - either cast nullptr to a Key const *,
        /// or explicitly specify a template argument.
        template <typename Key> reference getElement(Key const * = nullptr) {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

        /// @brief Const accessor by key type - either cast nullptr to a Key
        /// const *, or explicitly specify a template argument.
        template <typename Key>
        const_reference getElement(Key const * = nullptr) const {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return m_contents[index<Key>::value];
        }

        /// @brief Subscript operator, if you don't mind instantiating your key
        /// type.
        template <typename Key> reference operator[](Key &&) {
            return getElement<Key>();
        }

        /// @brief Const subscript operator, if you don't mind instantiating
        /// your key type.
        template <typename Key> const_reference operator[](Key &&) const {
            return getElement<Key>();
        }

      private:
        static const auto SIZE = length<key_types>::value;
        using container = std::array<value_type, SIZE>;
        container m_contents;
    };

    /// @brief Nonmember accessor for typepack::TypeKeyedMap - explicitly
    /// specify your key type.
    ///
    /// @relates typepack::HomogeneousTypeMap<>
    template <typename Key, typename KeyList, typename ValueType>
    inline auto get(TypeKeyedMap<KeyList, ValueType> &c) ->
        typename TypeKeyedMap<KeyList, ValueType>::reference {
        return c.template getElement<Key>();
    }

    /// @brief Nonmember const accessor for typepack::TypeKeyedMap -
    /// explicitly specify your key type.
    ///
    /// @relates typepack::HomogeneousTypeMap<>
    template <typename Key, typename KeyList, typename ValueType>
    inline auto get(TypeKeyedMap<KeyList, ValueType> const &c) ->
        typename TypeKeyedMap<KeyList, ValueType>::const_reference {
        return c.template getElement<Key>();
    }
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_TypeKeyedMap_h_GUID_29C6B905_4493_43AF_A811_71589CFD6FBC
