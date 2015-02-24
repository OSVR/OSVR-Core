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

// Standard includes
// - none

using namespace osvr::common::byte_order;

class FactoriadicGeneral : public ::testing::Test {
  public:
    template <typename FactoriadicNumber>
    void testFactoriadicNumber(IDType integerValue) {
        BOOST_STATIC_ASSERT(FactoriadicDigitsInRange<FactoriadicNumber>::value);
        ASSERT_EQ((FactoriadicToInteger<FactoriadicNumber>::value),
                  integerValue);
    }
};

TEST_F(FactoriadicGeneral, BasicFromFactoriadic) {
    testFactoriadicNumber<Factoriadic<0> >(0);
    testFactoriadicNumber<Factoriadic<1, 0> >(1);
    testFactoriadicNumber<Factoriadic<1, 0, 0> >(2);
    testFactoriadicNumber<Factoriadic<1, 1, 0> >(3);
    testFactoriadicNumber<Factoriadic<2, 0, 0> >(4);
    testFactoriadicNumber<Factoriadic<2, 1, 1, 0> >(15);
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