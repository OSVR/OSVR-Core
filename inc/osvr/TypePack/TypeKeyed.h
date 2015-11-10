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

#ifndef INCLUDED_TypeKeyed_h_GUID_49539B71_1F6F_4A84_B681_4FD65F87A2A8
#define INCLUDED_TypeKeyed_h_GUID_49539B71_1F6F_4A84_B681_4FD65F87A2A8

// Internal Includes
#include "T.h"
#include "Contains.h"
#include "FindFirst.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>
#include <tuple>

namespace osvr {
namespace typepack {
    // Forward declaration
    template <typename Derived> class TypeKeyedBase;

    namespace typekeyed_detail {
        /// traits class that MUST be specialized for each type-keyed
        /// container: should contain a `using type = ` declaration returning
        /// the value type corresponding to the given key type.
        template <typename Derived, typename Key>
        struct ValueTypeAtKeyTraits {};

        /// traits class that can be specialized for each type-keyed
        /// container: should contain a `using type = ` declaration providing
        /// the list of key types. Default implementation assumes member type
        /// name `key_types`
        template <typename Derived> struct KeyTypesTraits {
            using type = typename Derived::key_types;
        };

        /// Gets key types list for a given type-keyed container, using the
        /// traits class.
        template <typename Derived>
        using key_types = t_<KeyTypesTraits<Derived>>;

        /// Returns an integral_constant for whether a given key is valid for a
        /// given type-keyed container.
        template <typename Derived, typename Key>
        using valid_key = contains<key_types<Derived>, Key>;

        /// Gets the index of the key in a key list for a given type-keyed
        /// container
        template <typename Derived, typename Key>
        using index = find_first<key_types<Derived>, Key>;

        /// Gets the corresponding value type in a given type-keyed container
        /// for a given key type
        template <typename Derived, typename Key>
        using value_type_at_key = t_<ValueTypeAtKeyTraits<Derived, Key>>;

        /// Gets the corresponding reference to value type in a given type-keyed
        /// container for a given key type
        template <typename Derived, typename Key>
        using ref_type_at_key =
            t_<std::add_lvalue_reference<value_type_at_key<Derived, Key>>>;

        /// Gets the corresponding rvalue-reference to value type in a given
        /// type-keyed container for a given key type
        template <typename Derived, typename Key>
        using rref_type_at_key =
            t_<std::add_rvalue_reference<value_type_at_key<Derived, Key>>>;

        /// Gets the corresponding reference to constant value type in a given
        /// type-keyed container for a given key type
        template <typename Derived, typename Key>
        using cref_type_at_key = t_<std::add_lvalue_reference<
            t_<std::add_const<value_type_at_key<Derived, Key>>>>>;

        /// Class with static members performing the actual access. Specialize
        /// if  `std::get<index>()` doesn't work on your type-keyed structure's
        /// implementation or if it doesn't have a `nested_container()` member.
        ///
        ///
        /// Add <code>
        /// template <typename, typename>
        /// friend struct typekeyed_detail::ValueAccessor
        /// </code> to your
        /// and you can keep your `nested_container()` members private.
        template <typename Derived, typename Key> struct ValueAccessor {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            using reference = ref_type_at_key<Derived, Key>;
            using rvalue_reference = rref_type_at_key<Derived, Key>;
            using const_reference = cref_type_at_key<Derived, Key>;
            using Index = index<Derived, Key>;
            static reference get_reference(Derived &c) {
                return std::get<Index::value>(c.nested_container());
            }
            static const_reference get_const_reference(Derived const &c) {
                return std::get<Index::value>(c.nested_container());
            }
            static rvalue_reference get_rvalue_reference(Derived &&c) {
                return std::forward<rvalue_reference>(
                    std::get<Index::value>(c.nested_container()));
            }
        };

        // forward declaration
        template <typename Key, typename Derived>
        ref_type_at_key<Derived, Key> get(TypeKeyedBase<Derived> &c);

        // forward declaration
        template <typename Key, typename Derived>
        rref_type_at_key<Derived, Key> rget(TypeKeyedBase<Derived> &&c);

        // forward declaration
        template <typename Key, typename Derived>
        cref_type_at_key<Derived, Key> cget(TypeKeyedBase<Derived> const &c);
    } // namespace typekeyed_detail

    /// CRTP base for type-keyed data types, providing a unified interface
    /// (compile-time polymorphism) and shared functionality. Methods and
    /// related free functions work on all type-keyed containers derived from
    /// this base class.
    template <typename Derived> class TypeKeyedBase {
      public:
        using DerivedType = Derived;

        /// CRTP: access derived class.
        DerivedType &derived() { return *static_cast<DerivedType *>(this); }

        /// CRTP: const-access derived class.
        DerivedType const &derived() const {
            return *static_cast<DerivedType const *>(this);
        }

        /// CRTP: const-access derived class.
        DerivedType const &const_derived() const {
            return *static_cast<DerivedType const *>(this);
        }

        /// Get a reference to a value in the container, keyed by the provided
        /// type (either explicit or deduced from the argument).
        template <typename Key>
        typekeyed_detail::ref_type_at_key<Derived, Key>
        get(Key const * = nullptr) {
            return typekeyed_detail::get<Key>(*this);
        }

        /// Get a reference to a constant value in the container, keyed by the
        /// provided type (either explicit or deduced from the argument).
        template <typename Key>
        typekeyed_detail::cref_type_at_key<Derived, Key>
        get(Key const * = nullptr) const {
            return typekeyed_detail::cget<Key>(*this);
        }
    };

    namespace typekeyed_detail {
        /// Gets a reference to a value in a type-keyed container using the
        /// specified key type.
        template <typename Key, typename Derived>
        inline ref_type_at_key<Derived, Key> get(TypeKeyedBase<Derived> &c) {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            return ValueAccessor<Derived, Key>::get_reference(c.derived());
        }
        /// Gets a rvalue-reference to a value in a type-keyed container using
        /// the specified key type.
        template <typename Key, typename Derived>
        inline rref_type_at_key<Derived, Key> rget(TypeKeyedBase<Derived> &&c) {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            return std::forward<rref_type_at_key<Derived, Key>>(
                ValueAccessor<Derived, Key>::get_rvalue_reference(
                    std::forward<Derived &&>(c.derived())));
        }

        /// Gets an lvalue-reference to a constant value in a type-keyed
        /// container using the specified key type.
        template <typename Key, typename Derived>
        inline cref_type_at_key<Derived, Key>
        cget(const TypeKeyedBase<Derived> &c) {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            return ValueAccessor<Derived, Key>::get_const_reference(
                c.const_derived());
        }

    } // namespace typekeyed_detail

    /// Gets a reference to a value in a type-keyed container using the
    /// specified key type.
    ///
    /// @relates TypeKeyedBase
    template <typename Key, typename Derived>
    inline typekeyed_detail::ref_type_at_key<Derived, Key>
    get(TypeKeyedBase<Derived> &c) {
        return typekeyed_detail::get<Key>(c);
    }

    /// Gets an rvalue-reference to a value in a type-keyed container using the
    /// specified key type.
    ///
    /// @relates TypeKeyedBase
    template <typename Key, typename Derived>
    inline typekeyed_detail::rref_type_at_key<Derived, Key>
    get(TypeKeyedBase<Derived> &&c) {
        return std::forward<typekeyed_detail::rref_type_at_key<Derived, Key>>(
            typekeyed_detail::rget<Key>(c));
    }
    /// Gets a reference to a constant value in a type-keyed container using the
    /// specified key type.
    ///
    /// @relates TypeKeyedBase
    template <typename Key, typename Derived>
    inline typekeyed_detail::cref_type_at_key<Derived, Key>
    get(TypeKeyedBase<Derived> const &c) {
        return typekeyed_detail::cget<Key>(c);
    }
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_TypeKeyed_h_GUID_49539B71_1F6F_4A84_B681_4FD65F87A2A8
