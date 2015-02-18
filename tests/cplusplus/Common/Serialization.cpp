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
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using osvr::common::Buffer;

TEST(Serialization, BufferWrapperEmptyConstruction) {
    osvr::common::Buffer<> buf;
    ASSERT_EQ(buf.size(), 0);
    auto reader = buf.startReading();
    ASSERT_EQ(reader.bytesRead(), 0);
    ASSERT_EQ(reader.bytesRemaining(), 0);
    uint16_t val;
    ASSERT_THROW(reader.read(val), std::runtime_error);
    ASSERT_THROW(reader.readAligned(val, sizeof(val)), std::runtime_error);
}

template <typename T> class IntRawSerialization : public ::testing::Test {};
typedef ::testing::Types<uint64_t, int64_t, uint32_t, int32_t, uint16_t,
                         int16_t, uint8_t, int8_t> IntTypes;

TYPED_TEST_CASE(IntRawSerialization, IntTypes);

TYPED_TEST(IntRawSerialization, RoundTrip1) {
    Buffer<> buf;
    TypeParam inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TYPED_TEST(IntRawSerialization, RoundTrip0) {
    Buffer<> buf;
    TypeParam inVal(0);
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TYPED_TEST(IntRawSerialization, Endianness) {
    Buffer<> buf;
    TypeParam inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    auto zeroes = buf.size() - 1;
    for (size_t i = 0; i < zeroes; ++i) {
        ASSERT_EQ(buf.getContents()[i], uint8_t(0));
    }
    ASSERT_EQ(buf.getContents()[buf.size() - 1], uint8_t(1));
}

template <typename T>
class ArithmeticRawSerialization : public ::testing::Test {};
typedef ::testing::Types<double, uint64_t, int64_t, uint32_t, int32_t, uint16_t,
                         int16_t, uint8_t, int8_t> ArithmeticTypes;

TYPED_TEST_CASE(ArithmeticRawSerialization, ArithmeticTypes);

TYPED_TEST(ArithmeticRawSerialization, RoundTrip1) {
    Buffer<> buf;
    TypeParam inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TYPED_TEST(ArithmeticRawSerialization, RoundTrip0) {
    Buffer<> buf;
    TypeParam inVal(0);
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

#ifdef _MSC_VER
/// Disable "truncation of constant value" warning here.
#pragma warning(push)
#pragma warning(disable : 4309)
#endif
TYPED_TEST(ArithmeticRawSerialization, RoundTripBigger) {
    Buffer<> buf;
    TypeParam inVal = static_cast<TypeParam>(50.5); // OK that it will truncate
                                                    // for integers, since we
                                                    // compare inVal vs outVal,
                                                    // not the literal.
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TYPED_TEST(ArithmeticRawSerialization, RoundTripNegative) {
    Buffer<> buf;
    TypeParam inVal = static_cast<TypeParam>(-50.5); // OK that it will truncate
                                                     // for integers, since we
                                                     // compare inVal vs outVal,
                                                     // not the literal.
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    TypeParam outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}
#ifdef _MSC_VER
/// Disable "truncation of constant value" warning here.
#pragma warning(pop)
#endif

class MyClass {
  public:
    MyClass() : a(0), b(0), c(0) {}
    template <typename T> void processMessage(T &process) {

        process(a);
        process(b);
        process(c);
    }
    int32_t a;
    uint32_t b;
    int8_t c;
};

TEST(Serialization, SelfDescriptive) {
    Buffer<> buf;
    {
        MyClass data;
        data.a = 1;
        data.b = 2;
        data.c = 3;
        osvr::common::serialize(buf, data);
    }

    {
        MyClass data;
        auto reader = buf.startReading();
        osvr::common::deserialize(reader, data);
        ASSERT_EQ(data.a, 1);
        ASSERT_EQ(data.b, 2);
        ASSERT_EQ(data.c, 3);
    }
}