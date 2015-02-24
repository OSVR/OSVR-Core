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
#include <boost/mpl/if.hpp>

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

        namespace sequence {
            /// @brief A C++11 compile-time sequence of small integers
            template <ByteNumber... Elements> struct SmallIntSequence;

            /// @brief Get the head element of a SmallIntSequence: derives from
            /// the corresponding std::integral_constant
            template <typename Sequence> struct HeadElement;

            template <ByteNumber Head, ByteNumber... Tail>
            struct HeadElement<SmallIntSequence<Head, Tail...> >
                : std::integral_constant<ByteNumber, Head> {};

            template <ByteNumber Head>
            struct HeadElement<SmallIntSequence<Head> >
                : std::integral_constant<ByteNumber, Head> {};

            /// @brief Metafunction: Get the tail (cdr) of a SmallIntSequence,
            /// resulting new SmallIntSequence is in nested typedef `type`
            template <typename Sequence> struct GetTail;

            template <ByteNumber Head, ByteNumber... Tail>
            struct GetTail<SmallIntSequence<Head, Tail...> > {
                typedef SmallIntSequence<Tail...> type;
            };

            /// @brief Get the length of a SmallIntSequence: derives from the
            /// corresponding std::integral_constant
            template <typename Sequence> struct Length;

            template <>
            struct Length<void> : std::integral_constant<ByteNumber, 0> {};

            template <ByteNumber... Digits>
            struct Length<SmallIntSequence<Digits...> >
                : std::integral_constant<ByteNumber, sizeof...(Digits)> {};

            /// @brief Determines if there are any elements in a sequence tail
            /// (that is, length > 1): derives from the std::true_type or
            /// std::false_type. Useful for partial specialization in compile
            /// time algorithms.
            template <typename Number> struct HasTail;

            template <ByteNumber Head, ByteNumber... Tail>
            struct HasTail<SmallIntSequence<Head, Tail...> > : std::true_type {
            };

            template <ByteNumber Head>
            struct HasTail<SmallIntSequence<Head> > : std::false_type {};

            /// @brief Metafunction: pushes an element on to the head of the
            /// provided SmallIntSequence. Result returned in nested typedef
            /// `type`
            template <typename Sequence, ByteNumber Digit> struct PushHead;
            template <ByteNumber... Digits, ByteNumber Digit>
            struct PushHead<SmallIntSequence<Digits...>, Digit> {
                typedef SmallIntSequence<Digit, Digits...> type;
            };

            /// @brief Compile-time mechanism of a mixed compile-time/run-time
            /// algorithm for applying a functor starting at the beginning of a
            /// sequence
            template <typename Sequence, typename F,
                      bool = HasTail<Sequence>::value>
            struct VisitFromLeft;

            template <typename Sequence, typename F>
            struct VisitFromLeft<Sequence, F, true> {
                typedef typename GetTail<Sequence>::type Tail;
                static void apply(F &f) {
                    f(HeadElement<Sequence>::value);
                    VisitFromLeft<Tail, F>::apply(f);
                }
            };
            template <typename Sequence, typename F>
            struct VisitFromLeft<Sequence, F, false> {
                static void apply(F &f) { f(HeadElement<Sequence>::value); }
            };

            /// @brief Mixed compile-time/run-time algorithm for applying a
            /// functor starting at the beginning of a sequence
            template <typename Sequence, typename F>
            inline void visitFromLeft(F &f) {
                VisitFromLeft<Sequence, F>::apply(f);
            }
        } // namespace sequence
        namespace factoriadic {
            /// @brief A number in factoriadic number type, with each sequence
            /// element a digit. Includes the trailing 0.
            template <ByteNumber... Digits>
            using Factoriadic = sequence::SmallIntSequence<Digits...>;

            /// @brief The 0-based index, from the right, of the head digit
            /// (element)
            template <typename Number>
            struct HeadDigitIndex
                : std::integral_constant<ByteNumber,
                                         sequence::Length<Number>::value - 1> {
            };

            /// @brief The place value of the head digit (element)
            template <typename Number>
            struct HeadPlaceValue
                : detail::Factorial_c<IDType, HeadDigitIndex<Number>::value> {};

            /// @brief The value represented by the head digit (element),
            /// incorporating place value
            template <typename Number>
            struct HeadValue
                : std::integral_constant<
                      IDType, ((HeadPlaceValue<Number>::value) *
                               (sequence::HeadElement<Number>::value))> {};

            /// @brief Helper metafunction, performing boolean AND on
            /// std::integral_constant types
            template <typename A, typename B>
            struct BooleanAnd
                : std::integral_constant<bool, A::value && B::value> {};

            /// @brief Metafunction for determining if the head digit is in the
            /// appropriate range to be valid in a factoriadic number, inherits
            /// from the corresponding std::integral_constant<bool>
            template <typename Number>
            struct HeadDigitInRange
                : std::integral_constant<
                      bool, (sequence::HeadElement<Number>::value <=
                             HeadDigitIndex<Number>::value)> {};

            /// @brief Metafunction for determining if all digits are in the
            /// appropriate range to be valid in a factoriadic number, inherits
            /// from the corresponding std::integral_constant<bool>
            template <typename Number, bool = sequence::HasTail<Number>::value>
            struct DigitsInRange;

            template <typename Number>
            struct DigitsInRange<Number, true>
                : BooleanAnd<HeadDigitInRange<Number>,
                             DigitsInRange<
                                 typename sequence::GetTail<Number>::type> > {};

            template <typename Number>
            struct DigitsInRange<Number, false> : HeadDigitInRange<Number> {};

            /// @brief Compile-time conversion from a factoriadic number to an
            /// integer. Result returned by inheriting from the appropriate
            /// std::integral_constant
            template <typename Number, bool = sequence::HasTail<Number>::value>
            struct ToInteger;

            template <typename Number>
            struct ToInteger<Number, false> : HeadValue<Number> {
                BOOST_STATIC_ASSERT(HeadValue<Number>::value == 0);
            };

            template <typename Number>
            struct ToInteger<Number, true>
                : std::integral_constant<
                      IDType, HeadValue<Number>::value +
                                  ToInteger<typename sequence::GetTail<
                                      Number>::type>::value> {};

            struct FromIntegerDone {
                template <typename Number, IDType> struct apply {
                    typedef Number type;
                };
            };
            struct FromIntegerNextRound {
                template <typename Number, IDType Val> struct apply {
                    static const IDType Radix =
                        sequence::Length<Number>::value + 1;
                    static const IDType Remainder = Val % Radix;
                    typedef typename sequence::PushHead<Number, Remainder>::type
                        NewNumber;
                    static const IDType Quotient = Val / Radix;
                    typedef typename boost::mpl::if_c<
                        (Quotient > 0), FromIntegerNextRound,
                        FromIntegerDone>::type Iterate;
                    typedef
                        typename Iterate::template apply<NewNumber,
                                                         Quotient>::type type;
                };
            };

            /// @brief Metafunction converting from an integer value to a
            /// factoriadic value, result returned as nested typedef `type`
            template <IDType Val> struct FromInteger {
                typedef typename FromIntegerNextRound::template apply<
                    Factoriadic<>, Val>::type type;
            };

        } // namespace factoriadic

        namespace permutations {

            template <ByteNumber... Elements>
            using PermutationList = sequence::SmallIntSequence<Elements...>;

        } // namespace permutations

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
