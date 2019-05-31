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
#include <osvr/Util/UniqueContainer.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <string>
#include <type_traits>
#include <vector>

using osvr::util::UniqueContainer;
using std::is_same;
using std::string;
using std::vector;
namespace wrap_policies = osvr::util::container_policies;
namespace policies = osvr::util::unique_container_policies;

using container = UniqueContainer<vector<int>, policies::PushBack,
                                  wrap_policies::size, wrap_policies::empty>;

TEST_CASE("UniqueContainer-construct") {
    REQUIRE_NOTHROW((container{}));
    REQUIRE((container{}).empty());
}

TEST_CASE("UniqueContainer-addUnique") {
    container c;
    REQUIRE(c.insert(1));
    REQUIRE(c.insert(2));
    REQUIRE(c.size() == 2);
    REQUIRE(c.contains(1));
    REQUIRE(c.contains(2));
    REQUIRE_FALSE(c.contains(3));
}
TEST_CASE("UniqueContainer-addNonUnique") {
    container c;
    REQUIRE(c.insert(1));
    REQUIRE_FALSE(c.insert(1));
    REQUIRE(c.insert(2));
    REQUIRE_FALSE(c.insert(2));
    REQUIRE(c.size() == 2);
    REQUIRE(c.contains(1));
    REQUIRE(c.contains(2));
    REQUIRE_FALSE(c.contains(3));

    REQUIRE(c.insert(3));
    REQUIRE(c.contains(3));
    REQUIRE_FALSE(c.insert(3));
    REQUIRE(c.contains(3));
    REQUIRE(c.size() == 3);
}

TEST_CASE("UniqueContainer-removeUniqueFront") {
    container c;
    c.insert(1);
    c.insert(2);
    REQUIRE(c.size() == 2);

    REQUIRE(c.remove(1));
    REQUIRE(c.size() == 1);
    REQUIRE_FALSE(c.contains(1));
    REQUIRE(c.contains(2));

    REQUIRE(c.remove(2));
    REQUIRE(c.size() == 0);
    REQUIRE(c.empty());
    REQUIRE_FALSE(c.contains(1));
    REQUIRE_FALSE(c.contains(2));
}

TEST_CASE("UniqueContainer-removeUniqueBack") {
    container c;
    c.insert(1);
    c.insert(2);
    REQUIRE(c.size() == 2);

    REQUIRE(c.remove(2));
    REQUIRE(c.size() == 1);
    REQUIRE(c.contains(1));
    REQUIRE_FALSE(c.contains(2));

    REQUIRE(c.remove(1));
    REQUIRE(c.size() == 0);
    REQUIRE(c.empty());
    REQUIRE_FALSE(c.contains(1));
    REQUIRE_FALSE(c.contains(2));
}

TEST_CASE("UniqueContainer-removeNonUnique") {
    container c;
    c.insert(1);
    c.insert(2);
    REQUIRE(c.size() == 2);

    REQUIRE(c.remove(1));

    REQUIRE_FALSE(c.remove(1));
    REQUIRE(c.size() == 1);
    REQUIRE_FALSE(c.contains(1));
    REQUIRE(c.contains(2));
}
