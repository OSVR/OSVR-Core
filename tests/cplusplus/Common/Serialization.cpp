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
#include <osvr/Common/Buffer.h>
#include <osvr/Common/BufferTraits.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <string>
#include <type_traits>

using osvr::common::Buffer;

TEST_CASE("Buffer-EmptyConstructionAndInvalidRead") {
    osvr::common::Buffer<> buf;
    REQUIRE(buf.size() == 0);
    auto reader = buf.startReading();
    REQUIRE(reader.bytesRead() == 0);
    REQUIRE(reader.bytesRemaining() == 0);
    uint16_t val;
    REQUIRE_THROWS_AS(reader.read(val), std::runtime_error);
    REQUIRE_THROWS_AS(reader.readAligned(val, sizeof(val)), std::runtime_error);
}

TEST_CASE("Buffer-TypeTraits") {
    using osvr::common::BufferByteVector;
    using osvr::common::BufferReader;
    using osvr::common::is_buffer;
    using osvr::common::is_buffer_reader;

    REQUIRE(is_buffer<Buffer<> >::value);
    REQUIRE_FALSE(is_buffer<BufferReader<BufferByteVector> >::value);
    REQUIRE_FALSE(is_buffer<BufferByteVector>::value);

    REQUIRE_FALSE(is_buffer_reader<Buffer<> >::value);
    REQUIRE(is_buffer_reader<BufferReader<BufferByteVector> >::value);
    REQUIRE_FALSE(is_buffer_reader<BufferByteVector>::value);
}

TEMPLATE_TEST_CASE("IntRawSerialization-RoundTrip1", "", uint64_t, int64_t,
                   uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    TestType outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEMPLATE_TEST_CASE("IntRawSerialization-RoundTrip0", "", uint64_t, int64_t,
                   uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal(0);
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    TestType outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEMPLATE_TEST_CASE("IntRawSerialization-Endianness", "", uint64_t, int64_t,
                   uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    auto zeroes = buf.size() - 1;
    for (size_t i = 0; i < zeroes; ++i) {
        REQUIRE(buf.getContents()[i] == uint8_t(0));
    }
    REQUIRE(buf.getContents()[buf.size() - 1] == uint8_t(1));
}

TEMPLATE_TEST_CASE("ArithmeticRawSerialization-RoundTrip1", "", double,
                   uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t,
                   uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal(1);
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    TestType outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEMPLATE_TEST_CASE("ArithmeticRawSerialization-RoundTrip0", "", double,
                   uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t,
                   uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal(0);
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    TestType outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEMPLATE_TEST_CASE("ArithmeticRawSerialization-VectorRoundTrip", "", double,
                   uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t,
                   uint8_t, int8_t) {
    Buffer<> buf;
    TestType in(0);
    static const auto count = 5;
    std::vector<TestType> inVal;
    for (int i = 0; i < count; ++i) {
        in += 1.8;
        inVal.push_back(in);
    }

    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() >= sizeof(uint32_t) + count * sizeof(TestType));

    auto reader = buf.startReading();

    std::vector<TestType> outVal(1);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRemaining() == 0);
}

#ifdef _MSC_VER
/// Disable "truncation of constant value" warning here.
#pragma warning(push)
#pragma warning(disable : 4309)
#endif
TEMPLATE_TEST_CASE("ArithmeticRawSerialization-RoundTripBigger", "", double,
                   uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t,
                   uint8_t, int8_t) {
    Buffer<> buf;
    TestType inVal = static_cast<TestType>(50.5); // OK that it will truncate
                                                  // for integers, since we
                                                  // compare inVal vs outVal,
                                                  // not the literal.
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    TestType outVal(0);
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEMPLATE_TEST_CASE("ArithmeticRawSerialization-RoundTripNegative", "", double,
                   uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t,
                   uint8_t, int8_t) {
    // Only do the body of this test for signed types.
    if (std::is_signed<TestType>()) {
        Buffer<> buf;
        TestType inVal =
            static_cast<TestType>(-50.5); // OK that it will truncate
                                          // for integers, since we
                                          // compare inVal vs outVal,
                                          // not the literal.
        osvr::common::serialization::serializeRaw(buf, inVal);
        REQUIRE(buf.size() == sizeof(TestType));

        auto reader = buf.startReading();

        REQUIRE(reader.bytesRemaining() == sizeof(TestType));
        TestType outVal(0);
        osvr::common::serialization::deserializeRaw(reader, outVal);
        REQUIRE(inVal == outVal);
        REQUIRE(reader.bytesRead() == sizeof(TestType));
        REQUIRE(reader.bytesRemaining() == 0);
    }
}
#ifdef _MSC_VER
/// Disable "truncation of constant value" warning here.
#pragma warning(pop)
#endif
TEST_CASE("BoolSerialization-RoundTripTrue") {
    Buffer<> buf;
    typedef OSVR_CBool TestType;
    bool inVal = true;
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    bool outVal = false;
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEST_CASE("BoolSerialization-RoundTripFalse") {
    Buffer<> buf;
    typedef OSVR_CBool TestType;
    bool inVal = false;
    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() == sizeof(TestType));

    auto reader = buf.startReading();

    REQUIRE(reader.bytesRemaining() == sizeof(TestType));
    bool outVal = true;
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRead() == sizeof(TestType));
    REQUIRE(reader.bytesRemaining() == 0);
}

TEST_CASE("StringVectorSerialization-RoundTrip") {
    auto buf = Buffer<>{};
    using TestType = std::vector<std::string>;
    auto inVal = TestType{};
    std::ostringstream os;
    static const auto count = 10;
    for (int i = 0; i < count; ++i) {
        inVal.push_back(os.str());
        os << "x";
    }

    osvr::common::serialization::serializeRaw(buf, inVal);
    REQUIRE(buf.size() >= sizeof(uint32_t) + count * sizeof(char));

    auto reader = buf.startReading();

    auto outVal = TestType{};
    osvr::common::serialization::deserializeRaw(reader, outVal);
    REQUIRE(inVal == outVal);
    REQUIRE(reader.bytesRemaining() == 0);
}

class SerializationAlignment {
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
        REQUIRE(in32 == out32);
        REQUIRE(in8 == out8);
    }
    int8_t in8;
    int32_t in32;
    int8_t out8;
    int32_t out32;
    Buffer<> buf;
};

TEST_CASE_METHOD(SerializationAlignment,
                 "SerializationAlignment-Buffer32Then8") {
    osvr::common::serialization::serializeRaw(buf, in32);
    REQUIRE(buf.size() == sizeof(in32));
    osvr::common::serialization::serializeRaw(buf, in8);
    {
        INFO("No padding should be required here");
        REQUIRE(buf.size() == sizeof(in32) + sizeof(in8));
    }

    auto reader = buf.startReading();
    osvr::common::serialization::deserializeRaw(reader, out32);
    osvr::common::serialization::deserializeRaw(reader, out8);
    REQUIRE(reader.bytesRemaining() == 0);
    checkOutput();
}

TEST_CASE_METHOD(SerializationAlignment,
                 "SerializationAlignment-Buffer8Then32") {
    osvr::common::serialization::serializeRaw(buf, in8);
    REQUIRE(buf.size() == sizeof(in8));
    osvr::common::serialization::serializeRaw(buf, in32);
    {
        INFO("Should be padded out to 32bits");
        REQUIRE(buf.size() == 2 * sizeof(in32));
    }

    auto reader = buf.startReading();
    osvr::common::serialization::deserializeRaw(reader, out8);
    osvr::common::serialization::deserializeRaw(reader, out32);
    REQUIRE(reader.bytesRemaining() == 0);
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

TEST_CASE("Serialization-SelfDescriptive") {
    Buffer<> buf;
    {
        MyClass data;
        data.a = 1;
        data.b = 2;
        data.c = 3;
        osvr::common::serialize(buf, data);
    }
    {
        INFO("No padding should be needed here.");
        REQUIRE(buf.size() ==
                sizeof(int32_t) + sizeof(uint32_t) + sizeof(int8_t));
    }

    {
        MyClass data;
        auto reader = buf.startReading();
        osvr::common::deserialize(reader, data);
        REQUIRE(data.a == 1);
        REQUIRE(data.b == 2);
        REQUIRE(data.c == 3);
    }
}
