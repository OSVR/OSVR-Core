/** @file
    @brief Test Implementation

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

// Internal Includes
#include <osvr/Common/Endianness.h>
#include <osvr/Common/ByteOrderIdentifiers.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include "gtest/gtest.h"
#include <boost/static_assert.hpp>
#include <tinympl/is_sequence.hpp>

// Standard includes
// - none

using namespace osvr::common::byte_order;

inline void fromIntImpl(std::ostream &os, IDType num, IDType radix = 1) {
    auto remain = num % radix;
    auto quot = num / radix;
    if (quot > 0) {
        fromIntImpl(os, quot, radix + 1);
    }
    os << remain;
}
inline std::string fromInt(IDType num) {
    std::ostringstream ostr;
    fromIntImpl(ostr, num);
    return ostr.str();
}

class StringifySequence {
  public:
    template <typename T> void operator()(T num) { m_str << int(num); }

    std::string get() const { return m_str.str(); }

  private:
    std::ostringstream m_str;
};

template <typename Number> inline std::string stringifySequence() {
    StringifySequence f;
    sequence::visitFromLeft<Number>(f);
    return f.get();
}

class FactoriadicGeneral : public ::testing::Test {
  public:
    template <typename FactoriadicNumber, IDType integerValue>
    void testFactoriadicNumber() {
        BOOST_STATIC_ASSERT(FactoriadicDigitsInRange<FactoriadicNumber>::value);
        ASSERT_EQ((FactoriadicToInteger<FactoriadicNumber>::value),
                  integerValue);

        ASSERT_EQ(stringifySequence<FactoriadicNumber>(),
                  fromInt(integerValue));

        ASSERT_EQ(stringifySequence<FactoriadicNumber>(),
                  stringifySequence<
                      typename factoriadic::FromInteger<integerValue>::type>());
    }
};

TEST_F(FactoriadicGeneral, BasicFromFactoriadic) {
    testFactoriadicNumber<Factoriadic<0>, 0>();
    testFactoriadicNumber<Factoriadic<1, 0>, 1>();
    testFactoriadicNumber<Factoriadic<1, 0, 0>, 2>();
    testFactoriadicNumber<Factoriadic<1, 1, 0>, 3>();
    testFactoriadicNumber<Factoriadic<2, 0, 0>, 4>();
    testFactoriadicNumber<Factoriadic<2, 1, 1, 0>, 15>();
}

TEST(Sequence, Generation) {
    ASSERT_EQ(stringifySequence<sequence::GenerateRange<5>::type>(), "01234");
}

#if 1
#define COMPILE_TIME_ASSERT(X)                                                 \
    BOOST_STATIC_ASSERT(X);                                                    \
    ASSERT_TRUE(X)
#endif
#if 0
#define COMPILE_TIME_ASSERT(X) ASSERT_TRUE(X)
#endif
#if 0
#define COMPILE_TIME_ASSERT(X) BOOST_STATIC_ASSERT(X)
#endif

TEST(Sequence, Unique) {
    COMPILE_TIME_ASSERT(
        sequence::Length<sequence::SmallIntSequence<> >::value == 0);
    COMPILE_TIME_ASSERT(
        sequence::HasTail<sequence::SmallIntSequence<> >::value == false);

    COMPILE_TIME_ASSERT(
        sequence::Length<sequence::SmallIntSequence<5> >::value == 1);
    COMPILE_TIME_ASSERT(
        sequence::HasTail<sequence::SmallIntSequence<5> >::value == false);

    COMPILE_TIME_ASSERT(
        (sequence::Length<sequence::SmallIntSequence<5, 5> >::value == 2));
    //    COMPILE_TIME_ASSERT((sequence::AllUnique<sequence::SmallIntSequence<>
    //    >::value));
    ASSERT_TRUE((sequence::AllUnique<sequence::SmallIntSequence<1> >::value));
    ASSERT_TRUE((sequence::AllUnique<
        sequence::SmallIntSequence<1, 2, 3, 4, 5> >::value));
    ASSERT_FALSE((sequence::AllUnique<
        sequence::SmallIntSequence<1, 2, 3, 4, 4, 5> >::value));
}

TEST(Permutation, General) {
    typedef sequence::vector_c<ByteNumber, 1, 2, 3, 4, 5> SamplePermutation;
    typedef permutations::GenerateSequence<4>::type SequenceOfFour;
    ASSERT_TRUE((tinympl::is_sequence<
        sequence::SmallIntSequence<1, 2, 3, 4, 5> >::value));

    ASSERT_TRUE(
        (permutations::IsValidPermutation<SequenceOfFour>::type::value));
}

#if 0
template <typename T> class UIntEndiannessID : public ::testing::Test {};
typedef ::testing::Types<uint64_t, uint32_t, uint16_t> UIntTypes;

TYPED_TEST_CASE(UIntEndiannessID, UIntTypes);

TYPED_TEST(UIntEndiannessID, SwapHalves) {
    typedef osvr::common::serialization::detail::ByteOrders<sizeof(TypeParam)>
        ByteOrderStruct;
    using osvr::common::serialization::detail::SwapHalves;
    ASSERT_NE(ByteOrderStruct::BIG_ENDIAN, ByteOrderStruct::LITTLE_ENDIAN);
    // auto bigSwap = SwapHalves<ByteOrderStruct,
    // ByteOrderStruct::BIG_ENDIAN>::value;
    // ASSERT_EQ()
}
#endif