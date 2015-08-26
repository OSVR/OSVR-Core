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
#include "gtest/gtest.h"

// Standard includes
#include <type_traits>
#include <string>
#include <vector>
using std::vector;
using std::string;
using std::is_same;
using osvr::util::UniqueContainer;
namespace wrap_policies = osvr::util::container_policies;
namespace policies = osvr::util::unique_container_policies;

using container = UniqueContainer<vector<int>, policies::PushBack,
                                  wrap_policies::size, wrap_policies::empty>;

TEST(UniqueContainer, construct) {
    ASSERT_NO_THROW((container{}));
    ASSERT_TRUE((container{}).empty());
}

TEST(UniqueContainer, addUnique) {
    container c;
    ASSERT_TRUE(c.insert(1));
    ASSERT_TRUE(c.insert(2));
    ASSERT_EQ(c.size(), 2);
    ASSERT_TRUE(c.contains(1));
    ASSERT_TRUE(c.contains(2));
    ASSERT_FALSE(c.contains(3));
}
TEST(UniqueContainer, addNonUnique) {
    container c;
    ASSERT_TRUE(c.insert(1));
    ASSERT_FALSE(c.insert(1));
    ASSERT_TRUE(c.insert(2));
    ASSERT_FALSE(c.insert(2));
    ASSERT_EQ(c.size(), 2);
    ASSERT_TRUE(c.contains(1));
    ASSERT_TRUE(c.contains(2));
    ASSERT_FALSE(c.contains(3));

    ASSERT_TRUE(c.insert(3));
    ASSERT_TRUE(c.contains(3));
    ASSERT_FALSE(c.insert(3));
    ASSERT_TRUE(c.contains(3));
    ASSERT_EQ(c.size(), 3);
}

TEST(UniqueContainer, removeUniqueFront) {
    container c;
    c.insert(1);
    c.insert(2);
    ASSERT_EQ(c.size(), 2);

    ASSERT_TRUE(c.remove(1));
    ASSERT_EQ(c.size(), 1);
    ASSERT_FALSE(c.contains(1));
    ASSERT_TRUE(c.contains(2));

    ASSERT_TRUE(c.remove(2));
    ASSERT_EQ(c.size(), 0);
    ASSERT_TRUE(c.empty());
    ASSERT_FALSE(c.contains(1));
    ASSERT_FALSE(c.contains(2));
}

TEST(UniqueContainer, removeUniqueBack) {
    container c;
    c.insert(1);
    c.insert(2);
    ASSERT_EQ(c.size(), 2);

    ASSERT_TRUE(c.remove(2));
    ASSERT_EQ(c.size(), 1);
    ASSERT_TRUE(c.contains(1));
    ASSERT_FALSE(c.contains(2));

    ASSERT_TRUE(c.remove(1));
    ASSERT_EQ(c.size(), 0);
    ASSERT_TRUE(c.empty());
    ASSERT_FALSE(c.contains(1));
    ASSERT_FALSE(c.contains(2));
}

TEST(UniqueContainer, removeNonUnique) {
    container c;
    c.insert(1);
    c.insert(2);
    ASSERT_EQ(c.size(), 2);

    ASSERT_TRUE(c.remove(1));

    ASSERT_FALSE(c.remove(1));
    ASSERT_EQ(c.size(), 1);
    ASSERT_FALSE(c.contains(1));
    ASSERT_TRUE(c.contains(2));
}
