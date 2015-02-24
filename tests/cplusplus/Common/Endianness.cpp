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