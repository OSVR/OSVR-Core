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
#include "SerializationTraitExample_Complicated.h"
#include "SerializationTraitExample_Simple.h"
#include <osvr/Common/Buffer.h>
#include <osvr/Common/Serialization.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <string>

using namespace osvr::common;

TEMPLATE_TEST_CASE("SerializationTraitsExamples-CanSerialize", "",
                   YourSimpleType, YourComplicatedType) {
    Buffer<> buf;
    TestType input = {1.5, 300, -5};
    REQUIRE_NOTHROW(osvr::common::serialization::serializeRaw(buf, input));
}

TEMPLATE_TEST_CASE("SerializationTraitsExamples-SerializeMatchesSize", "",
                   YourSimpleType, YourComplicatedType) {
    Buffer<> buf;
    TestType input = {1.5, 300, -5};
    osvr::common::serialization::serializeRaw(buf, input);
    REQUIRE_NOTHROW(serialization::getBufferSpaceRequiredRaw(0, input));
    REQUIRE(buf.size() == serialization::getBufferSpaceRequiredRaw(0, input));
}

TEMPLATE_TEST_CASE("SerializationTraitsExamples-RoundTrip", "", YourSimpleType,
                   YourComplicatedType) {
    Buffer<> buf;
    TestType input = {1.5, 300, -5};
    osvr::common::serialization::serializeRaw(buf, input);

    auto reader = buf.startReading();
    TestType output;
    REQUIRE_NOTHROW(serialization::deserializeRaw(reader, output));
    REQUIRE(input.A == output.A);
    REQUIRE(input.B == output.B);
    REQUIRE(input.C == output.C);
}
