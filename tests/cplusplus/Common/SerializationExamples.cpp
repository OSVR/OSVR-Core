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
#include "SerializationTraitExample_Simple.h"
#include "SerializationTraitExample_Complicated.h"

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using namespace osvr::common;

template <typename T>
class SerializationTraitsExamples : public ::testing::Test {};
typedef ::testing::Types<YourSimpleType, YourComplicatedType> ExampleTypes;

TYPED_TEST_CASE(SerializationTraitsExamples, ExampleTypes);

TYPED_TEST(SerializationTraitsExamples, CanSerialize) {
    Buffer<> buf;
    TypeParam input = {1.5, 300, -5};
    ASSERT_NO_THROW(osvr::common::serialization::serializeRaw(buf, input));
}

TYPED_TEST(SerializationTraitsExamples, SerializeMatchesSize) {
    Buffer<> buf;
    TypeParam input = {1.5, 300, -5};
    osvr::common::serialization::serializeRaw(buf, input);
    ASSERT_NO_THROW(serialization::getBufferSpaceRequiredRaw(0, input));
    ASSERT_EQ(buf.size(), serialization::getBufferSpaceRequiredRaw(0, input));
}

TYPED_TEST(SerializationTraitsExamples, RoundTrip) {
    Buffer<> buf;
    TypeParam input = {1.5, 300, -5};
    osvr::common::serialization::serializeRaw(buf, input);

    auto reader = buf.startReading();
    TypeParam output;
    ASSERT_NO_THROW(serialization::deserializeRaw(reader, output));
    ASSERT_EQ(input.A, output.A);
    ASSERT_EQ(input.B, output.B);
    ASSERT_EQ(input.C, output.C);
}
