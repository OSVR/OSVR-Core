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
#include <tinympl/as_sequence.hpp>
#include <tinympl/vector.hpp>
#include <tinympl/generate_n.hpp>
#include <tinympl/size.hpp>
#include <tinympl/all_of.hpp>
#include <tinympl/logical_and.hpp>
#include <tinympl/max_element.hpp>
#include <tinympl/at.hpp>
#include <tinympl/equal_to.hpp>
#include <tinympl/minus.hpp>
#include <tinympl/is_unique.hpp>

#include <tinympl/count_if.hpp>
#include <tinympl/greater.hpp>

// Standard includes
#include <type_traits>

namespace osvr {
namespace common {
    namespace byte_order {

        typedef uint8_t ByteNumber;
        typedef uint16_t IDType;

        template <ByteNumber i>
        using bytenum = std::integral_constant<ByteNumber, i>;

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

            /// @brief Metafunction converting some integral_constant to a
            /// bytenum.
            template <typename IntC> struct ConvertToByteNumber {
                typedef bytenum<IntC::value> type;
            };

            /// @brief Get the length of a SmallIntSequence: derives from the
            /// corresponding std::integral_constant
            template <typename Sequence> struct Length;

            template <>
            struct Length<void> : ::std::integral_constant<ByteNumber, 0> {};

            template <ByteNumber... Elements>
            struct Length<SmallIntSequence<Elements...> >
                : std::integral_constant<ByteNumber, sizeof...(Elements)> {};

            /// @brief Determines if there are any elements in a sequence tail
            /// (that is, length > 1): derives from the std::true_type or
            /// std::false_type. Useful for partial specialization in compile
            /// time algorithms.
            template <typename Sequence, bool = (Length<Sequence>::value > 1)>
            struct HasTail;

            template <typename Sequence>
            struct HasTail<Sequence, true> : std::true_type {};

            template <typename Sequence>
            struct HasTail<Sequence, false> : std::false_type {};

            /// @brief Determines if there are any elements in a sequence
            /// (that is, length > 0): derives from the std::true_type or
            /// std::false_type. Useful for partial specialization in compile
            /// time algorithms.
            template <typename Sequence, bool = (Length<Sequence>::value > 0)>
            struct HasElements;

            template <typename Sequence>
            struct HasElements<Sequence, true> : std::true_type {};

            template <typename Sequence>
            struct HasElements<Sequence, false> : std::false_type {};

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

            /// @brief Metafunction: pushes an element on to the head of the
            /// provided SmallIntSequence. Result returned in nested typedef
            /// `type`
            template <typename Sequence, ByteNumber Digit> struct PushFront;
            template <ByteNumber... Digits, ByteNumber Digit>
            struct PushFront<SmallIntSequence<Digits...>, Digit> {
                typedef SmallIntSequence<Digit, Digits...> type;
            };

            /// @brief Metafunction: pushes an element on to the back of the
            /// provided SmallIntSequence. Result returned in nested typedef
            /// `type`
            template <typename Sequence, ByteNumber Digit> struct PushBack;
            template <ByteNumber... Digits, ByteNumber Digit>
            struct PushBack<SmallIntSequence<Digits...>, Digit> {
                typedef SmallIntSequence<Digits..., Digit> type;
            };

            /// @brief Metafunction: generates an integer range with the
            /// specified number of elements, in order starting from 0.
            template <ByteNumber DesiredDigits,
                      typename Sequence = SmallIntSequence<>,
                      bool = (DesiredDigits == Length<Sequence>::value)>
            struct GenerateRange;

            template <ByteNumber DesiredDigits, typename Sequence>
            struct GenerateRange<DesiredDigits, Sequence, true> {
                typedef Sequence type;
            };

            template <ByteNumber DesiredDigits, typename Sequence>
            struct GenerateRange<DesiredDigits, Sequence, false>
                : GenerateRange<DesiredDigits,
                                typename PushBack<
                                    Sequence, Length<Sequence>::value>::type> {
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

            /// @brief Checks to see if an element exists in the sequence
            template <typename Sequence, ByteNumber Element,
                      bool = HasElements<Sequence>::value, typename = void>
            struct Contains;

            // base case: empty sequence
            template <typename Sequence, ByteNumber Element>
            struct Contains<Sequence, Element, false, void> : std::false_type {
            };

            // base case: head is desired element
            template <typename Sequence, ByteNumber Element>
            struct Contains<Sequence, Element, true,
                            typename std::enable_if<
                                Element == HeadElement<Sequence>::value>::type>
                : std::true_type {};
            // recursive case: head not desired element, but sequence not empty
            template <typename Sequence, ByteNumber Element>
            struct Contains<Sequence, Element, true,
                            typename std::enable_if<
                                Element != HeadElement<Sequence>::value>::type>
                : Contains<typename GetTail<Sequence>::type, Element> {};

            /// @brief Checks to see if all sequence elements are unique.
            template <typename Sequence, typename = void> struct AllUnique;

            // Base case: empty or singleton
            template <typename Sequence>
            struct AllUnique<Sequence, typename std::enable_if<(
                                           Length<Sequence>::value < 2)>::type>
                : std::true_type {};

            // Base case: head is in the tail.
            template <typename Sequence>
            struct AllUnique<
                Sequence,
                typename std::enable_if<(
                    HasTail<Sequence>::value &&
                    Contains<typename GetTail<Sequence>::type,
                             HeadElement<Sequence>::value>::value)>::type>
                : std::false_type {};

            // Recursive case: head is unique, check the tail.
            template <typename Sequence>
            struct AllUnique<
                Sequence,
                typename std::enable_if<(
                    HasTail<Sequence>::value &&
                    !Contains<typename GetTail<Sequence>::type,
                              HeadElement<Sequence>::value>::value)>::type>
                : AllUnique<typename GetTail<Sequence>::type> {};

            template <typename ValueType, typename Accum, ValueType... Elements>
            struct make_vector_c_impl;

            // recursive case: got an element to push_back
            template <typename ValueType, typename Accum, ValueType Head,
                      ValueType... Tail>
            struct make_vector_c_impl<ValueType, Accum, Head, Tail...> {
                typedef typename Accum::template push_back<
                    std::integral_constant<ByteNumber, Head> >::type NewAccum;
                typedef typename make_vector_c_impl<ValueType, NewAccum,
                                                    Tail...>::type type;
            };

            // base case: no more elements left
            template <typename ValueType, typename Accum>
            struct make_vector_c_impl<ValueType, Accum> {
                typedef Accum type;
            };

            /// @brief Metafunction to generate a tinympl::vector<> of
            /// integral_constants
            template <typename ValueType, ValueType... Elements>
            struct make_vector_c
                : make_vector_c_impl<ValueType, tinympl::vector<>,
                                     Elements...> {};
            /// @brief Inherits from a tinympl::vector containing the integral
            /// constants you specified.
            template <typename ValueType, ValueType... Elements>
            struct vector_c : make_vector_c<ValueType, Elements...>::type {};

            template <template <class...> class Op, typename State,
                      typename... Args>
            struct left_consume_var;
            template <template <class...> class Op, typename State,
                      typename Head, typename... Tail>
            struct left_consume_var<Op, State, Head, Tail...> {
                typedef typename left_consume_var<
                    Op, typename Op<State, Head, Tail...>::type, Tail...>::type
                    type;
            };
            template <template <class...> class Op, typename State>
            struct left_consume_var<Op, State> {
                typedef State type;
            };

            /// @brief Like left_fold but passed the entire remaining sequence
            /// each time, with state kept separate.
            template <template <class...> class Op, typename State,
                      typename Sequence>
            struct left_consume
                : left_consume<Op, State, tinympl::as_sequence_t<Sequence> > {};
            template <template <class...> class Op, typename State,
                      typename... Elements>
            struct left_consume<Op, State, tinympl::sequence<Elements...> >
                : left_consume_var<Op, State, Elements...> {};

            /// @brief Compile-time mechanism of a mixed compile-time/run-time
            /// algorithm for applying a functor starting at the beginning of a
            /// sequence
            template <typename F, typename... Args>
            struct left_fold_runtime_impl;

            template <typename F, typename Head, typename... Tail>
            struct left_fold_runtime_impl<F, Head, Tail...> {
                static void apply(F &f) {
                    f.operator()<Head>();
                    left_fold_runtime_impl<F, Tail...>::apply(f);
                }
            };
            template <typename F> struct left_fold_runtime_impl<F> {
                static void apply(F &) {}
            };
            template <typename F, typename... Args>
            struct left_fold_runtime_impl<F, tinympl::sequence<Args...> >
                : left_fold_runtime_impl<F, Args...> {};

            template <typename Sequence, typename F>
            inline void left_fold_runtime(F &f) {
                left_fold_runtime_impl<
                    F, tinympl::as_sequence_t<Sequence> >::apply(f);
            }

        } // namespace sequence
    }     // namespace byte_order
} // namespace common
} // namespace osvr

namespace tinympl {
/// @brief Provide conversion from our home-grown variadic compile-time integer
/// sequence to tinympl sequences.
template <osvr::common::byte_order::ByteNumber... Elements>
struct as_sequence<
    osvr::common::byte_order::sequence::SmallIntSequence<Elements...> > {
    typedef typename osvr::common::byte_order::sequence::make_vector_c<
        osvr::common::byte_order::ByteNumber, Elements...>::type type;
};
}
namespace osvr {
namespace common {
    namespace byte_order {
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
                    typedef
                        typename sequence::PushFront<Number, Remainder>::type
                            NewNumber;
                    static const IDType Quotient = Val / Radix;
                    typedef typename std::conditional<
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
            template <ByteNumber NumBytes>
            struct GenerateSequence
                : tinympl::generate_n<NumBytes, sequence::ConvertToByteNumber,
                                      tinympl::vector> {};
            template <typename Val> struct IsByteNumber {
                typedef std::is_same<ByteNumber, typename Val::value_type> type;
            };

            template <typename Sequence> struct CorrectMaxValue {
                typedef tinympl::size<Sequence> size;
                typedef typename tinympl::max_element<Sequence>::type max_index;
                typedef typename tinympl::at<max_index::value, Sequence>::type
                    max_value;
                typedef typename tinympl::equal_to<
                    typename tinympl::minus<size, bytenum<1> >::type,
                    max_value>::type type; // Max value is size-1
            };
            template <typename Sequence> struct IsValidPermutation {
                typedef typename tinympl::and_<
                    typename tinympl::all_of<Sequence, IsByteNumber>::
                        type, // all ByteNumber elements
                    typename CorrectMaxValue<Sequence>::type,
                    typename tinympl::is_unique<Sequence>::type // all elements
                                                                // unique
                    >::type type;
            };
            template <typename Head, typename... Tail> struct NumLargerThan {};

            template <typename State, typename Head, typename... Tail>
            struct ToFactoriadicConsumeOp {

                template <typename Val>
                using LargerThanHead = tinympl::greater<Val, Head>;
                using NumLargerThanHead =
                    typename tinympl::count_if<tinympl::sequence<Tail...>,
                                               LargerThanHead>::type;
                typedef typename sequence::ConvertToByteNumber<
                    NumLargerThanHead>::type NewElement;
                typedef
                    typename State::template push_back<NewElement>::type type;
            };

            /// @brief Convert a sequence showing a permutation into a
            /// factoriadic number (Lehmer code)
            template <typename Permutation>
            struct ToFactoriadic
                : sequence::left_consume<ToFactoriadicConsumeOp,
                                         tinympl::vector<>, Permutation> {};

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
