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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Endianness_h_GUID_A8D4BB43_3F1B_46AA_8044_BD082A07C299
#define INCLUDED_Endianness_h_GUID_A8D4BB43_3F1B_46AA_8044_BD082A07C299

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Common/IntegerByteSwap.h>

// Library/third-party includes
#include <boost/version.hpp>

#if BOOST_VERSION >= 105500
#include <boost/predef.h>
#if BOOST_ENDIAN_LITTLE_BYTE
#define OSVR_IS_LITTLE_ENDIAN
#define OSVR_BYTE_ORDER_ABCD
#elif BOOST_ENDIAN_BIG_BYTE
#define OSVR_IS_BIG_ENDIAN
#define OSVR_BYTE_ORDER_DCBA
#else
#error "Unsupported byte order!"
#endif

#else
#include <boost/detail/endian.hpp>

#if defined(BOOST_LITTLE_ENDIAN)
#define OSVR_IS_LITTLE_ENDIAN
#define OSVR_BYTE_ORDER_ABCD
#elif defined(BOOST_BIG_ENDIAN)
#define OSVR_IS_BIG_ENDIAN
#define OSVR_BYTE_ORDER_DCBA
#else
#error "Unsupported byte order!"
#endif

#endif

#include <boost/integer.hpp>

// Standard/system includes
#include <type_traits>

#if defined(__FLOAT_WORD_ORDER) && defined(__BYTE_ORDER)
#if (__FLOAT_WORD_ORDER != __BYTE_ORDER)
/// Handle weird mixed-order doubles like some ARM systems.
#define OSVR_FLOAT_ORDER_MIXED
#endif
#endif

#include <string.h>

namespace osvr {
namespace common {

    namespace serialization {
        template <typename T, typename Dummy = void>
        struct NetworkByteOrderTraits;

        /// @brief Take the binary representation of a value with one type,
        /// and return it as another type, without breaking strict aliasing.
        template <typename Dest, typename Src>
        inline Dest safe_pun(Src const src) {
            static_assert(sizeof(Src) == sizeof(Dest),
                          "Can only use safe_pun for types of the same size");
            Dest ret;
            memcpy(reinterpret_cast<char *>(&ret),
                   reinterpret_cast<char const *>(&src), sizeof(src));
            return ret;
        }

        namespace detail {
            /// @brief Stock implementation of a no-op host-network conversion
            template <typename T> struct NoOpHostNetworkConversion {
                static T hton(T const v) { return v; }
                static T ntoh(T const v) { return v; }
            };
            /// @brief Stock implementation of a byte-swapping host-network
            /// conversion
            template <typename T> struct IntegerByteOrderSwap {
                static T hton(T const v) { return integerByteSwap(v); }
                static T ntoh(T const v) { return hton(v); }
            };
            /// @brief Stock implementation of a type-punning host-network
            /// conversion
            template <typename T, typename UnsignedIntType>
            struct TypePunByteOrder {

                typedef T type;
                typedef UnsignedIntType uint_t;

                static_assert(sizeof(T) == sizeof(UnsignedIntType),
                              "Type-punning host-network conversion only works "
                              "when the types are the same size");
                static_assert(
                    std::is_unsigned<UnsignedIntType>::value,
                    "Template parameter UnsignedIntType should be unsigned!");

                typedef NetworkByteOrderTraits<uint_t> IntTraits;
                static type hton(type const v) {
                    return safe_pun<type>(IntTraits::hton(safe_pun<uint_t>(v)));
                }
                static type ntoh(type const v) { return hton(v); }
            };

        } // namespace detail

#if defined(OSVR_IS_BIG_ENDIAN)
        template <typename T>
        struct NetworkByteOrderTraits<
            T, typename std::enable_if<std::is_integral<T>::value>::type>
            : detail::NoOpHostNetworkConversion<T> {};

#elif defined(OSVR_IS_LITTLE_ENDIAN)
        template <typename T>
        struct NetworkByteOrderTraits<
            T, typename std::enable_if<std::is_integral<T>::value>::type>
            : detail::IntegerByteOrderSwap<T> {};
#endif

#if 0
        /// @todo not sure how to do this on arm mixed-endian.
        template<>
        struct NetworkByteOrderTraits<float, void> : detail::TypePunByteOrder<float, uint32_t> {
        };
#endif

#if defined(OSVR_FLOAT_ORDER_MIXED)
        template <> struct NetworkByteOrderTraits<double, void> {
            typedef detail::TypePunByteOrder<double, uint64_t> ByteOrder;
            typedef ByteOrder::type type;
            static const size_t WORD_SIZE = sizeof(type) / 2;
            static inline type hton(type const v) {
                /// First change byte order as needed.
                type src = ByteOrder::hton(v);
                type ret;
                /// Now swap word order
                memcpy(reinterpret_cast<char *>(&ret),
                       reinterpret_cast<char const *>(&src) + WORD_SIZE,
                       WORD_SIZE);
                memcpy(reinterpret_cast<char *>(&ret) + WORD_SIZE,
                       reinterpret_cast<char const *>(&src), WORD_SIZE);
                return ret;
            }
            static type ntoh(type const v) { return hton(v); }
        };
#else
        template <>
        struct NetworkByteOrderTraits<double, void>
            : detail::TypePunByteOrder<double, uint64_t> {};
#endif

        /// @brief Convert network byte order (big endian) to host byte order
        template <typename T> inline T ntoh(T const v) {
            return NetworkByteOrderTraits<T>::ntoh(v);
        }

        /// @brief Convert host byte order to network byte order (big endian)
        template <typename T> inline T hton(T const v) {
            return NetworkByteOrderTraits<T>::hton(v);
        }
    } // namespace serialization
} // namespace common
} // namespace osvr

#endif // INCLUDED_Endianness_h_GUID_A8D4BB43_3F1B_46AA_8044_BD082A07C299
