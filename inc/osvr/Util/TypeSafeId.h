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

// Library/third-party includes
// - none

// Standard includes
#include <limits>

namespace osvr {
namespace util {
    /// @brief Namespace for traits templates associated with
    /// ::osvr::util::TypeSafeId
    namespace typesafeid_traits {
        /// @brief Explicitly specialize for your tag type if you want a
        /// different
        /// underlying type.
        template <typename Tag> struct WrappedType { typedef uint32_t type; };

        /// @brief Explicitly specialize for your tag type if you want a
        /// different signal value for invalid/empty: default is the maximum
        /// representable value for your type.
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
    /// @tparam Tag any type - does not have to be defined, just declared (so
    /// `struct mytag;` somewhere is fine). The tag serves to make integer IDs
    /// have distinct types, and also serves as a look-up key in the
    /// ::osvr::util::typesafeid_traits classes for underlying integer type and
    /// sentinel empty/invalid value.
    ///
    /// Initial implementation inspired by
    /// http://www.ilikebigbits.com/blog/2014/5/6/type-safe-identifiers-in-c
    /// though this version now strays quite far by strengthening type-safety
    /// and encapsulation, and by using traits classes to specify details based
    /// on tag type alone.
    template <class Tag> class TypeSafeId {
      public:
        /// @brief The type of the current class.
        typedef TypeSafeId<Tag> type;

        /// @brief The contained/wrapped type.
        typedef typename typesafeid_traits::WrappedType<Tag>::type wrapped_type;

        /// @brief Static factory method to return an invalid/empty ID.
        static type invalid() { return type(); }

        // Default constructor which will set m_val to the empty/invalid value.
        TypeSafeId() : m_val(sentinel()) {}

        // Explicit constructor from the wrapped type
        explicit TypeSafeId(wrapped_type val) : m_val(val) {}

        /// @brief Check whether the ID is empty/invalid
        bool empty() const { return m_val == sentinel(); }

        /// @brief Read-only accessor to the (non-type-safe!) wrapped value
        wrapped_type value() const { return m_val; }

        /// @brief Reference accessor to the (non-type-safe!) wrapped value
        wrapped_type & value() { return m_val; }

      private:
        /// @brief Utility function to access the SentinelValue trait.
        static wrapped_type sentinel() {
            return typesafeid_traits::SentinelValue<Tag>::get();
        }
        wrapped_type m_val;
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
