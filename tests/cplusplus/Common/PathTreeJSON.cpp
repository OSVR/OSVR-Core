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
#include "DummyTree.h"
#include <osvr/Common/PathTreeSerialization.h>

#include <test_path_tree_json.h>

// Library/third-party includes
#include "gtest/gtest.h"
#include "json/reader.h"

// Standard includes
// - none

namespace common = osvr::common;
using osvr::common::PathTree;

std::string getJSON() {
    return std::string(test_path_tree_json, sizeof(test_path_tree_json));
}

TEST(PathTreeJSON, EmptyTreeToJson) {
    PathTree tree;
    ASSERT_NO_THROW(common::pathTreeToJson(tree));
    auto json = common::pathTreeToJson(tree);
    ASSERT_EQ(json.size(), 0);
}

TEST(PathTreeJSON, EmptyJsonArrayToTree) {

    PathTree tree;
    Json::Value val(Json::arrayValue);

    ASSERT_NO_THROW(common::jsonToPathTree(tree, val));
    ASSERT_EQ(val, common::pathTreeToJson(tree));
}

TEST(PathTreeJSON, ManualTreeToJsonWithRoundtrip) {
    PathTree tree;
    setupDummyTree(tree);

    ASSERT_NO_THROW(common::pathTreeToJson(tree));
    auto json = common::pathTreeToJson(tree);
    std::cout << json.toStyledString() << std::endl;
    ASSERT_NE(json.size(), 0u);

    PathTree tree2;
    ASSERT_NO_THROW(common::jsonToPathTree(tree2, json));
    ASSERT_EQ(json, common::pathTreeToJson(tree2));
}

TEST(PathTreeJSON, SerializedTreeToJsonWithRoundtrip) {

    Json::Value val;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(getJSON(), val));

    PathTree tree;
    ASSERT_NO_THROW(common::jsonToPathTree(tree, val));

    ASSERT_EQ(common::pathTreeToJson(tree), val);
}
