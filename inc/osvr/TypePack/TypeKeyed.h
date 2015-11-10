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
        /// traits class that needs to be specialized for each type-keyed
        /// container: should contain a `using type = ` declaration providing
        /// the list of key types.
        template <typename Derived> struct KeyTypesTraits;
        /// traits class that needs to be specialized for each type-keyed
        /// container: should contain a `using type = ` declaration providing
        /// the internal container type used.
        template <typename Derived> struct NestedContainerTraits;
        /// traits class that needs to be specialized for each type-keyed
        /// container: should contain a `using type = ` declaration returning
        /// the value type corresponding to the given key type.
        template <typename Derived, typename Key> struct ValueTypeAtKeyTraits;

        /// Gets key types list for a given type-keyed container, using the
        /// traits class.
        template <typename Derived>
        using key_types = t_<KeyTypesTraits<Derived>>;

        /// Gets the nested container type for a given type-keyed container,
        /// using the traits class.
        template <typename Derived>
        using nested_container = t_<NestedContainerTraits<Derived>>;

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

        /// Class that exists solely as a way to expose the nested container to
        /// the type-keyed container functionality without making it public.
        ///
        /// Add `friend class typekeyed_detail::ContainerAccessor` and you can
        /// provide private `nested_container()` methods.
        class ContainerAccessor {
            template <typename Derived>
            static auto getNestedContainer(Derived &c)
                -> nested_container<Derived> & {
                return c.nested_container();
            }
            template <typename Derived>
            static auto getNestedConstContainer(Derived const &c)
                -> nested_container<Derived> const & {
                return c.nested_container();
            }

            template <typename Derived, typename Key>
            friend struct ValueAccessor;
            template <typename Derived, typename Key>
            friend struct ConstValueAccessor;
        };

        /// Class that should have a static `apply()` member taking a reference
        /// to a container and returning a reference to the value. Specialize if
        /// `std::get<index>()` doesn't work on your implementation or if you
        /// don't have a `nested_container()` member.
        template <typename Derived, typename Key> struct ValueAccessor {
            static ref_type_at_key<Derived, Key> apply(Derived &c) {
                using Index = index<Derived, Key>;
                return std::get<Index::value>(
                    ContainerAccessor::getNestedContainer<Derived>(c));
            }
        };
        /// Class that should have a static `apply()` member taking a reference
        /// to a container and returning a reference to the value. Specialize if
        /// `std::get<index>()` doesn't work on your implementation or if you
        /// don't have a `nested_container()` member.
        template <typename Derived, typename Key> struct RValueAccessor {
            static rref_type_at_key<Derived, Key> apply(Derived &&c) {
                using Index = index<Derived, Key>;
                return std::forward<rref_type_at_key<Derived, Key>>(
                    std::get<Index::value>(
                        ContainerAccessor::getNestedContainer<Derived>(c)));
            }
        };
        /// Class that should have a static `apply()` member taking a ref to a
        /// const container and returning a ref to the const value. Specialize
        /// if `std::get<index>()` doesn't work on your implementation or if you
        /// don't have a `nested_container()` member.
        template <typename Derived, typename Key> struct ConstValueAccessor {
            static cref_type_at_key<Derived, Key> apply(Derived const &c) {
                using Index = index<Derived, Key>;
                return std::get<Index::value>(
                    ContainerAccessor::getNestedConstContainer<Derived>(c));
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
    /// (compile-time polymorphism) and shared functionality. Methods work on
    /// all type-keyed containers.
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
            return ValueAccessor<Derived, Key>::apply(c.derived());
        }
        /// Gets a rvalue-reference to a value in a type-keyed container using
        /// the specified key type.
        template <typename Key, typename Derived>
        inline rref_type_at_key<Derived, Key> rget(TypeKeyedBase<Derived> &&c) {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            return std::forward<rref_type_at_key<Derived, Key>>(
                RValueAccessor<Derived, Key>::apply(
                    std::forward<Derived &&>(c.derived())));
        }

        /// Gets an lvalue-reference to a constant value in a type-keyed
        /// container using the specified key type.
        template <typename Key, typename Derived>
        inline cref_type_at_key<Derived, Key>
        cget(const TypeKeyedBase<Derived> &c) {
            static_assert(valid_key<Derived, Key>::value,
                          "Key type not found in the list!");
            return ConstValueAccessor<Derived, Key>::apply(c.const_derived());
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
