/** @file
    @brief Header providing optimized integer byte-swap functionality through a
   single function template, implemented using platform special functions or
   intrinsics where possible.

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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_IntegerByteSwap_h_GUID_11841216_B020_47A7_D0A9_9B1CFFEB1C5C
#define INCLUDED_IntegerByteSwap_h_GUID_11841216_B020_47A7_D0A9_9B1CFFEB1C5C

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Common/ConfigByteSwapping.h>

// Library/third-party includes
#include <boost/integer.hpp>

// Standard includes
#include <type_traits>

#if defined(OSVR_HAVE_INTRIN_H) && defined(OSVR_HAVE_WORKING_MS_BYTESWAPS)
#include <intrin.h>
#endif

#ifdef OSVR_HAVE_BYTESWAP_H
#include <byteswap.h>
#endif

namespace osvr {
namespace common {
    namespace detail {
        /// @brief Tag type used only for recognizing ByteSwap specializations
        /// that aren't explicitly specialized.
        struct UnspecializedByteSwapBase {};

        /// @brief Template for providing/describing optimized/intrinsic byte
        /// swap functions.
        template <typename T> struct ByteSwap : UnspecializedByteSwapBase {};

        /// @brief Trait: For a given T=ByteSwap<ValueType> struct, is
        /// ByteSwap<ValueType> default/unspecialized?
        template <typename T>
        using IsByteSwapperUnspecialized = std::integral_constant<
            bool, std::is_base_of<UnspecializedByteSwapBase, T>::value>;

        /// @brief Trait: For a given T=ByteSwap<ValueType> struct, is
        /// ByteSwap<ValueType> specialized?
        template <typename T>
        using IsByteSwapperSpecialized =
            std::integral_constant<bool, !IsByteSwapperUnspecialized<T>::value>;

        struct NoOpFunction {
            template <typename ArgType> static ArgType apply(ArgType const v) {
                return v;
            }
        };

        /// @brief no-op swap for 1 byte.
        template <> struct ByteSwap<uint8_t> : NoOpFunction {};

        /// @brief no-op swap for 1 byte.
        template <> struct ByteSwap<int8_t> : NoOpFunction {};
#if defined(OSVR_HAVE_WORKING_MS_BYTESWAPS)
        template <> struct ByteSwap<uint16_t> {
            static uint16_t apply(uint16_t const v) {
                return _byteswap_ushort(v);
            }
        };

        template <> struct ByteSwap<uint32_t> {
            static uint32_t apply(uint32_t const v) {
                return _byteswap_ulong(v);
            }
        };
        template <> struct ByteSwap<uint64_t> {
            static uint64_t apply(uint64_t const v) {
                return _byteswap_uint64(v);
            }
        };
#elif defined(OSVR_HAVE_BYTESWAP_H) && defined(OSVR_HAVE_WORKING_BSWAP)
        template <> struct ByteSwap<uint16_t> {
            static uint16_t apply(uint16_t const v) { return bswap16(v); }
        };
        template <> struct ByteSwap<uint32_t> {
            static uint32_t apply(uint32_t const v) { return bswap32(v); }
        };
        template <> struct ByteSwap<uint64_t> {
            static uint64_t apply(uint64_t const v) { return bswap64(v); }
        };
#elif defined(OSVR_HAVE_BYTESWAP_H) &&                                         \
    defined(OSVR_HAVE_WORKING_BSWAP_UNDERSCORE)
        template <> struct ByteSwap<uint16_t> {
            static uint16_t apply(uint16_t const v) { return bswap_16(v); }
        };

        template <> struct ByteSwap<uint32_t> {
            static uint32_t apply(uint32_t const v) { return bswap_32(v); }
        };
        template <> struct ByteSwap<uint64_t> {
            static uint64_t apply(uint64_t const v) { return bswap_64(v); }
        };
#elif defined(OSVR_HAVE_BYTESWAP_H) &&                                         \
    defined(OSVR_HAVE_WORKING_UNDERSCORES_BSWAP)
        template <> struct ByteSwap<uint16_t> {
            static uint16_t apply(uint16_t const v) { return __bswap_16(v); }
        };

        template <> struct ByteSwap<uint32_t> {
            static uint32_t apply(uint32_t const v) { return __bswap_32(v); }
        };
        template <> struct ByteSwap<uint64_t> {
            static uint64_t apply(uint64_t const v) { return __bswap_64(v); }
        };
#endif
        /// @brief Universal byte-swap function that always works, but isn't
        /// necessarily optimized.
        template <typename T> inline T genericByteSwap(T const v) {
            T ret;
            for (size_t i = 0; i < sizeof(T); ++i) {
                reinterpret_cast<char *>(&ret)[i] =
                    reinterpret_cast<char const *>(&v)[sizeof(T) - 1 - i];
            }
            return ret;
        }

        /// @brief Traits assisting selection of optimized integer byte
        /// swapping.
        /// @tparam T an integer type, without const, volatile, or reference
        /// qualifiers.
        template <typename T> struct IntegerByteSwapTraits {

            typedef typename boost::int_t<sizeof(T) * 8>::exact int_t;
            typedef typename boost::uint_t<sizeof(T) * 8>::exact uint_t;
            typedef
                typename std::conditional<std::is_signed<T>::value, uint_t,
                                          int_t>::type opposite_signedness_type;
            typedef typename std::add_const<opposite_signedness_type>::type
                const_opposite_signedness_type;

            typedef ByteSwap<T> ByteSwapper;
            typedef ByteSwap<opposite_signedness_type> OppositeByteSwapper;

            static const bool HAVE_BYTESWAPPER =
                IsByteSwapperSpecialized<ByteSwapper>::value;
            static const bool HAVE_OPPPOSITEBYTESWAPPER =
                IsByteSwapperSpecialized<OppositeByteSwapper>::value;
        };

        /// @brief Implementation of integerByteSwap(): Gets called when we
        /// have an exact match to an explicit specialization
        template <typename T>
        inline T integerByteSwapImpl(
            T const v,
            typename std::enable_if<
                IntegerByteSwapTraits<T>::HAVE_BYTESWAPPER>::type * = nullptr) {
            typedef IntegerByteSwapTraits<T> Traits;
            return Traits::ByteSwapper::apply(v);
        }

        /// @brief Implementation of integerByteSwap(): Gets called when we
        /// do not have an exact match to an explicit specialization, but we
        /// do have a match if we switch the signedness.
        template <typename T>
        inline T integerByteSwapImpl(
            T const v,
            typename std::enable_if<
                (!IntegerByteSwapTraits<T>::HAVE_BYTESWAPPER) &&
                (IntegerByteSwapTraits<T>::HAVE_OPPPOSITEBYTESWAPPER)>::type * =
                nullptr) {
            typedef IntegerByteSwapTraits<T> Traits;
            return static_cast<T>(Traits::OppositeByteSwapper::apply(
                static_cast<typename Traits::const_opposite_signedness_type>(
                    v)));
        }

        /// @brief Implementation of integerByteSwap(): Gets called if no
        /// explicit specialization exists (for the type or its
        /// opposite-signedness relative)
        template <typename T>
        inline T integerByteSwapImpl(
            T const v,
            typename std::enable_if<
                (!IntegerByteSwapTraits<T>::HAVE_BYTESWAPPER) &&
                (!IntegerByteSwapTraits<T>::HAVE_OPPPOSITEBYTESWAPPER)>::
                type * = nullptr) {
            return genericByteSwap(v);
        }
    } // namespace detail

    /// @brief Swap the order of bytes of an arbitrary integer type
    ///
    /// @internal
    /// Primarily a thin wrapper to centralize type manipulation before
    /// calling a detail::IntegerByteSwapImpl::apply() overload selected by
    /// enable_if.
    template <typename Type> inline Type integerByteSwap(Type const v) {
        static_assert(std::is_integral<Type>::value,
                      "Can only apply integerByteSwap to integers");
        typedef typename std::remove_cv<
            typename std::remove_reference<Type>::type>::type T;

        return detail::integerByteSwapImpl<T>(v);
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_IntegerByteSwap_h_GUID_11841216_B020_47A7_D0A9_9B1CFFEB1C5C
