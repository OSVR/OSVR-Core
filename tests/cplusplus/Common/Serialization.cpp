/** @file
    @brief Test Implementation

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

// Internal Includes
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Common/BufferTraits.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using osvr::common::Buffer;

TEST(Buffer, EmptyConstructionAndInvalidRead) {
    osvr::common::Buffer<> buf;
    ASSERT_EQ(buf.size(), 0);
    auto reader = buf.startReading();
    ASSERT_EQ(reader.bytesRead(), 0);
    ASSERT_EQ(reader.bytesRemaining(), 0);
    uint16_t val;
    ASSERT_THROW(reader.read(val), std::runtime_error);
    ASSERT_THROW(reader.readAligned(val, sizeof(val)), std::runtime_error);
}

TEST(Buffer, TypeTraits) {
    using osvr::common::is_buffer;
    using osvr::common::is_buffer_reader;
    using osvr::common::BufferReader;
    using osvr::common::BufferByteVector;

    ASSERT_TRUE(is_buffer<Buffer<> >::value);
    ASSERT_FALSE(is_buffer<BufferReader<BufferByteVector> >::value);
    ASSERT_FALSE(is_buffer<BufferByteVector>::value);

    ASSERT_FALSE(is_buffer_reader<Buffer<> >::value);
    ASSERT_TRUE(is_buffer_reader<BufferReader<BufferByteVector> >::value);
    ASSERT_FALSE(is_buffer_reader<BufferByteVector>::value);
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

TYPED_TEST(ArithmeticRawSerialization, VectorRoundTrip) {
    Buffer<> buf;
    TypeParam in(0);
    static const auto count = 5;
    std::vector<TypeParam> inVal;
    for (int i = 0; i < count; ++i) {
        in += 1.8;
        inVal.push_back(in);
    }

    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_GE(buf.size(), sizeof(uint32_t) + count * sizeof(TypeParam));

    auto reader = buf.startReading();

    std::vector<TypeParam> outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
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
TEST(BoolSerialization, RoundTripTrue) {
    Buffer<> buf;
    typedef OSVR_CBool TypeParam;
    bool inVal = true;
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    bool outVal = false;
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TEST(BoolSerialization, RoundTripFalse) {
    Buffer<> buf;
    typedef OSVR_CBool TypeParam;
    bool inVal = false;
    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_EQ(buf.size(), sizeof(TypeParam));

    auto reader = buf.startReading();

    ASSERT_EQ(reader.bytesRemaining(), sizeof(TypeParam));
    bool outVal = true;
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRead(), sizeof(TypeParam));
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

TEST(StringVectorSerialization, RoundTrip) {
    auto buf = Buffer<>{};
    using TypeParam = std::vector<std::string>;
    auto inVal = TypeParam{};
    std::ostringstream os;
    static const auto count = 10;
    for (int i = 0; i < count; ++i) {
        inVal.push_back(os.str());
        os << "x";
    }

    osvr::common::serialization::serializeRaw(buf, inVal);
    ASSERT_GE(buf.size(), sizeof(uint32_t) + count * sizeof(char));

    auto reader = buf.startReading();

    auto outVal = TypeParam{};
    osvr::common::serialization::deserializeRaw(reader, outVal);
    ASSERT_EQ(inVal, outVal);
    ASSERT_EQ(reader.bytesRemaining(), 0);
}

class SerializationAlignment : public ::testing::Test {
  public:
    virtual void SetUp() {
        /// Give them some dummy values.
        in8 = 8;
        in32 = 32;
        /// Zero the outputs
        out8 = 0;
        out32 = 0;
    }

    void checkOutput() {
        ASSERT_EQ(in32, out32);
        ASSERT_EQ(in8, out8);
    }
    int8_t in8;
    int32_t in32;
    int8_t out8;
    int32_t out32;
    Buffer<> buf;
};

TEST_F(SerializationAlignment, Buffer32Then8) {
    osvr::common::serialization::serializeRaw(buf, in32);
    ASSERT_EQ(buf.size(), sizeof(in32));
    osvr::common::serialization::serializeRaw(buf, in8);
    ASSERT_EQ(buf.size(), sizeof(in32) + sizeof(in8))
        << "No padding should be required here";

    auto reader = buf.startReading();
    osvr::common::serialization::deserializeRaw(reader, out32);
    osvr::common::serialization::deserializeRaw(reader, out8);
    ASSERT_EQ(reader.bytesRemaining(), 0);
    checkOutput();
}

TEST_F(SerializationAlignment, Buffer8Then32) {
    osvr::common::serialization::serializeRaw(buf, in8);
    ASSERT_EQ(buf.size(), sizeof(in8));
    osvr::common::serialization::serializeRaw(buf, in32);
    ASSERT_EQ(buf.size(), 2 * sizeof(in32)) << "Should be padded out to 32bits";

    auto reader = buf.startReading();
    osvr::common::serialization::deserializeRaw(reader, out8);
    osvr::common::serialization::deserializeRaw(reader, out32);
    ASSERT_EQ(reader.bytesRemaining(), 0);
    checkOutput();
}

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

    ASSERT_EQ(buf.size(), sizeof(int32_t) + sizeof(uint32_t) + sizeof(int8_t))
        << "No padding should be needed here.";

    {
        MyClass data;
        auto reader = buf.startReading();
        osvr::common::deserialize(reader, data);
        ASSERT_EQ(data.a, 1);
        ASSERT_EQ(data.b, 2);
        ASSERT_EQ(data.c, 3);
    }
}
