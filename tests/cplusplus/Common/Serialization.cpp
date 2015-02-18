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

TEST(Serialization, RawSerialization) {}

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