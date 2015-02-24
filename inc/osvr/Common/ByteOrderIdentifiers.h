/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_ByteOrderIdentifiers_h_GUID_2885F7C2_270D_45BB_5E58_540132EAF449
#define INCLUDED_ByteOrderIdentifiers_h_GUID_2885F7C2_270D_45BB_5E58_540132EAF449

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace common {
    namespace byte_order {

        typedef uint8_t ByteNumber;
        typedef uint16_t IDType;
        namespace detail {
            template <typename T, T N, typename dummy = void>
            struct Factorial_c;

            template <typename T, T N>
            struct Factorial_c<T, N, typename std::enable_if<N != 0>::type> {
                static const T value = N * Factorial_c<T, N - 1>::value;
            };
            template <typename T, T N>
            struct Factorial_c<T, N, typename std::enable_if<N == 0>::type> {
                static const T value = 1;
            };
        } // namespace detail
        namespace factoriadic {
            template <ByteNumber... Elements> struct Factoriadic;

            template <typename Number> struct HeadDigit;

            template <ByteNumber Head, ByteNumber... Tail>
            struct HeadDigit<Factoriadic<Head, Tail...> >
                : std::integral_constant<ByteNumber, Head> {};

            template <ByteNumber Head>
            struct HeadDigit<Factoriadic<Head> >
                : std::integral_constant<ByteNumber, Head> {};

            template <typename Number> struct GetTail;

            template <ByteNumber Head, ByteNumber... Tail>
            struct GetTail<Factoriadic<Head, Tail...> > {
                typedef Factoriadic<Tail...> type;
            };
            /*
            template <ByteNumber Head> struct GetTail<Factoriadic<Head> > {
                // typedef Factoriadic<Tail...> type;
            };
            */

            template <typename Number> struct DigitCount;

            template <>
            struct DigitCount<void> : std::integral_constant<ByteNumber, 0> {};

            template <ByteNumber... Digits>
            struct DigitCount<Factoriadic<Digits...> >
                : std::integral_constant<ByteNumber, sizeof...(Digits)> {};

            template <typename Number>
            struct HeadDigitIndex
                : std::integral_constant<ByteNumber,
                                         DigitCount<Number>::value - 1> {};

            template <typename Number>
            struct HeadPlaceValue
                : detail::Factorial_c<IDType, HeadDigitIndex<Number>::value> {};

            template <typename Number>
            struct HeadValue
                : std::integral_constant<IDType,
                                         ((HeadPlaceValue<Number>::value) *
                                          (HeadDigit<Number>::value))> {};

            template <typename Number> struct HasTail;

            template <ByteNumber Head, ByteNumber... Tail>
            struct HasTail<Factoriadic<Head, Tail...> > : std::true_type {};

            template <ByteNumber Head>
            struct HasTail<Factoriadic<Head> > : std::false_type {};

            template <typename A, typename B>
            struct BooleanAnd
                : std::integral_constant<bool, A::value && B::value> {};

            template <typename Number>
            struct HeadDigitInRange
                : std::integral_constant<bool,
                                         (HeadDigit<Number>::value <=
                                          HeadDigitIndex<Number>::value)> {};

            template <typename Number, bool = HasTail<Number>::value>
            struct DigitsInRange;

            template <typename Number>
            struct DigitsInRange<Number, true>
                : BooleanAnd<HeadDigitInRange<Number>,
                             DigitsInRange<typename GetTail<Number>::type> > {};

            template <typename Number>
            struct DigitsInRange<Number, false> : HeadDigitInRange<Number> {};

            template <typename A, typename B>
            struct Sum : std::integral_constant<typename A::type,
                                                A::value + B::value> {};

            template <typename Number, bool = HasTail<Number>::value>
            struct ToInteger;

            template <typename Number>
            struct ToInteger<Number, false> : HeadValue<Number> {
                BOOST_STATIC_ASSERT(HeadValue<Number>::value == 0);
            };

            template <typename Number>
            struct ToInteger<Number, true>
                : std::integral_constant<
                      IDType,
                      HeadValue<Number>::value +
                          ToInteger<typename GetTail<Number>::type>::value> {};
        } // namespace factoriadic

        using factoriadic::Factoriadic;

        template <typename Number>
        struct FactoriadicToInteger : factoriadic::ToInteger<Number> {};

        template <typename FactoriadicNumber>
        struct FactoriadicDigitsInRange
            : factoriadic::DigitsInRange<FactoriadicNumber> {};
    } // namespace byte_order
} // namespace common
} // namespace osvr
#endif // INCLUDED_ByteOrderIdentifiers_h_GUID_2885F7C2_270D_45BB_5E58_540132EAF449
