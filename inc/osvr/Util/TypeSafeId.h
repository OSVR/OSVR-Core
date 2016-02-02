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

#ifndef INCLUDED_TypeSafeId_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2
#define INCLUDED_TypeSafeId_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Util/BasicTypeTraits.h>

// Library/third-party includes
// - none

// Standard includes
#include <limits>

namespace osvr {
namespace util {

    template <class Tag> class TypeSafeId;
    template <class Tag> class TypeSafeIdBase;
    template <class Tag> class TypeSafeIdRefAccessorBase;
    /// @brief Namespace for traits templates associated with
    /// ::osvr::util::TypeSafeId
    namespace typesafeid_traits {
        /// @brief Explicitly specialize for your tag type if you want a
        /// different underlying type.
        template <typename Tag> struct WrappedType { typedef uint32_t type; };

        /// Class for your specialization of ProvideReferenceAccessor
        /// to inherit from if you want to provide a non-const l-value reference
        /// accessor, weakening the typesafety of this wrapper.
        struct ShouldHaveReferenceAccessor {
            enum { value = true };
        };

        /// Explicitly specialize this for your tag type to derive from
        /// ProvideLValueReferenceAccessor if you want a `value()` member that
        /// returns a non-const reference (an l-value reference) - which does
        /// make it slightly less typesafe. For example,
        /// <code>
        /// template<> struct ProvideReferenceAccessor<MyTag> :
        /// ShouldHaveReferenceAccessor {};
        /// </code>
        template <typename Tag> struct ProvideReferenceAccessor {
            enum { value = false };
        };

        /// Selects one of the base classes based on whether we need that
        /// reference accessor.
        template <typename Tag> struct ComputeBaseClass {
            typedef typename Conditional<ProvideReferenceAccessor<Tag>::value,
                                         TypeSafeIdRefAccessorBase<Tag>,
                                         TypeSafeIdBase<Tag> >::type type;
        };

        /// @brief Explicitly specialize for your tag type if you want a
        /// different signal value for invalid/empty: default is the
        /// maximum representable value for your type.
        template <typename Tag> struct SentinelValue {
            typedef typename WrappedType<Tag>::type wrapped_type;
            static wrapped_type get() {
                return std::numeric_limits<wrapped_type>::max();
            }
        };

    } // namespace typesafeid_traits

    /// @brief A generic typesafe (as long as you use differing tag types)
    /// wrapper for identifiers (typically integers).
    ///
    /// @tparam Tag any type - does not have to be defined, just declared
    /// (so `struct mytag;` somewhere is fine). The tag serves to make integer
    /// IDs have distinct types, and also serves as a look-up key in the
    /// ::osvr::util::typesafeid_traits classes for underlying integer type
    /// and sentinel empty/invalid value.
    ///
    /// Initial implementation inspired by
    /// http://www.ilikebigbits.com/blog/2014/5/6/type-safe-identifiers-in-c
    /// though this version now strays quite far by strengthening
    /// type-safety and encapsulation, and by using traits classes to specify
    /// details based on tag type alone.
    template <class Tag> class TypeSafeIdBase {
      public:
        /// @brief The "public" type of the current class.
        typedef TypeSafeId<Tag> type;

        /// @brief The contained/wrapped type.
        typedef typename typesafeid_traits::WrappedType<Tag>::type wrapped_type;

        /// @brief Static factory method to return an invalid/empty ID.
        static type invalid() { return type(); }

        /// Default constructor which will set m_val to the empty/invalid
        /// value.
        TypeSafeIdBase() : m_val(sentinel()) {}

        /// Explicit constructor from the wrapped type
        explicit TypeSafeIdBase(wrapped_type val) : m_val(val) {}

        /// Copy constructor
        TypeSafeIdBase(TypeSafeIdBase const &other) : m_val(other.m_val) {}

        /// @brief Check whether the ID is empty/invalid
        bool empty() const { return m_val == sentinel(); }

        /// @brief Read-only accessor to the (non-type-safe!) wrapped value
        wrapped_type value() const { return m_val; }

      protected:
        /// @brief Utility function to access the SentinelValue trait.
        static wrapped_type sentinel() {
            return typesafeid_traits::SentinelValue<Tag>::get();
        }
        wrapped_type m_val;
    };

    template <class Tag>
    class TypeSafeIdRefAccessorBase : public TypeSafeIdBase<Tag> {
      public:
        typedef TypeSafeIdBase<Tag> Base;
        typedef typename typesafeid_traits::WrappedType<Tag>::type wrapped_type;
        TypeSafeIdRefAccessorBase() : Base() {}
        explicit TypeSafeIdRefAccessorBase(wrapped_type val) : Base(val) {}
        TypeSafeIdRefAccessorBase(TypeSafeIdRefAccessorBase const &other)
            : Base(other) {}

        /// @brief Non-const reference accessor to the (non-type-safe!)
        /// wrapped value - only available if specifically provided for by a tag
        /// specialization of traits.
        wrapped_type &value() { return Base::m_val; }
    };

    template <class Tag>
    class TypeSafeId : public typesafeid_traits::ComputeBaseClass<Tag>::type {
      public:
        /// @brief The type of the current class.
        typedef TypeSafeId<Tag> type;
        /// @brief The implementation base.
        typedef typename typesafeid_traits::ComputeBaseClass<Tag>::type Base;
        typedef typename typesafeid_traits::WrappedType<Tag>::type wrapped_type;
        TypeSafeId() : Base() {}
        explicit TypeSafeId(wrapped_type val) : Base(val) {}
        TypeSafeId(TypeSafeId const &other) : Base(other) {}
    };

    /// @brief Equality comparison operator for type-safe IDs
    /// @relates ::osvr::util::TypeSafeId
    template <typename Tag>
    inline bool operator==(TypeSafeId<Tag> const a, TypeSafeId<Tag> const b) {
        return a.value() == b.value();
    }

    /// @brief Inequality comparison operator for type-safe IDs
    /// @relates ::osvr::util::TypeSafeId
    template <typename Tag>
    inline bool operator!=(TypeSafeId<Tag> const a, TypeSafeId<Tag> const b) {
        return a.value() != b.value();
    }

} // namespace util
} // namespace osvr

#endif // INCLUDED_TypeSafeId_h_GUID_137CA336_382A_4796_7735_4521F02D5AC2
