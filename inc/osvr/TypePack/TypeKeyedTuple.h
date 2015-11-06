/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
// TypePack is part of OSVR-Core.
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4
#define INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4

// Internal Includes
#include "Contains.h"
#include "FindFirst.h"
#include "Apply.h"
#include "Transform.h"
#include "ApplyList.h"

// Library/third-party includes
// - none

// Standard includes
#include <tuple>

namespace osvr {
namespace typepack {

    namespace detail {
        struct make_tuple {
            template <typename... Ts> struct apply {
                using type = std::tuple<Ts...>;
            };
        };
    } // namespace detail

    /// @brief Provides a data structure where a value of heterogeneous data
    /// types may be stored at runtime for each of the "key" types in a list.
    /// The runtime data type stored is computed by an alias class.
    ///
    /// Vaguely replaces the functionality of a boost::fusion::map.
    ///
    /// Values can be accessed with a templated getElement member function, a
    /// subscript operator overload, as well as the nonmember get() free
    /// function templates.
    ///
    /// Element access performance is equal to `get()` on a std::tuple (should
    /// be constant)
    template <typename KeyList, typename ComputeValueTypes>
    class TypeKeyedTuple {
        using value_types = transform<KeyList, ComputeValueTypes>;

      public:
        using container_type = apply_list<quote<std::tuple>, value_types>;
        using key_types = KeyList;

        template <typename Key>
        using value_type_for = apply<ComputeValueTypes, Key>;
        template <typename Key>
        using value_reference_for = value_type_for<Key> &;
        template <typename Key>
        using value_const_reference_for = value_type_for<Key> const &;
        template <typename Key> using valid_key = contains<key_types, Key>;
        template <typename Key> using index = find_first<key_types, Key>;

        /// @brief Accessor by key type - either cast nullptr to a Key const *,
        /// or explicitly specify a template argument.
        template <typename Key>
        auto getElement(Key const * = nullptr) -> value_reference_for<Key> {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return std::get<index<Key>::value>(container_);
        }

        /// @brief Const accessor by key type - either cast nullptr to a Key
        /// const *, or explicitly specify a template argument.
        template <typename Key>
        auto getElement(Key const * = nullptr) const
            -> value_const_reference_for<Key> {
            static_assert(valid_key<Key>::value,
                          "Key type not found in the list!");
            return std::get<index<Key>::value>(container_);
        }

        /// @brief Subscript operator, if you don't mind instantiating your key
        /// type.
        template <typename Key>
        auto operator[](Key && ) -> decltype(getElement<Key>()) {
            return getElement<Key>();
        }

        /// @brief Const subscript operator, if you don't mind instantiating
        /// your key type.
        template <typename Key>
        auto operator[](Key &&) const -> decltype(getElement<Key>()) {
            return getElement<Key>();
        }

      private:
        container_type container_;
    };

    /// @brief Nonmember accessor for typepack::TypeKeyedTuple - explicitly
    /// specify your key type.
    template <typename Key, typename KeyList, typename ComputeValueTypes>
    inline auto get(TypeKeyedTuple<KeyList, ComputeValueTypes> &c)
        -> decltype(c.template getElement<Key>()) {
        return c.template getElement<Key>();
    }

    /// @brief Nonmember const accessor for typepack::TypeKeyedTuple -
    /// explicitly specify your key type.
    template <typename Key, typename KeyList, typename ComputeValueTypes>
    inline auto get(TypeKeyedTuple<KeyList, ComputeValueTypes> const &c)
        -> decltype(c.template getElement<Key>()) {
        return c.template getElement<Key>();
    }
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_TypeKeyedTuple_h_GUID_04E79266_BD2F_458D_B7AC_DF5F35CC6EC4
