/** @file
    @brief Test Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
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
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include "IsType.h"

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
// - none

using namespace osvr::common;

TEST_CASE("PathElement-getTypeName") {
    REQUIRE(std::string(elements::getTypeName<elements::NullElement>()) == "NullElement");
}

TEST_CASE("PathElement-isElementType") {
    REQUIRE(isElementType<elements::NullElement>(elements::NullElement()));
    REQUIRE_FALSE(
        isElementType<elements::DeviceElement>(elements::NullElement()));
}
