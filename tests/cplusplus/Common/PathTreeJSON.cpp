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
#include "json/reader.h"
#include <catch2/catch.hpp>

// Standard includes
// - none

namespace common = osvr::common;
using osvr::common::PathTree;

std::string getJSON() {
    return std::string(test_path_tree_json, sizeof(test_path_tree_json));
}

TEST_CASE("PathTreeJSON-EmptyTreeToJson") {
    PathTree tree;
    REQUIRE_NOTHROW(common::pathTreeToJson(tree));
    auto json = common::pathTreeToJson(tree);
    REQUIRE(json.size() == 0);
}

TEST_CASE("PathTreeJSON-EmptyJsonArrayToTree") {

    PathTree tree;
    Json::Value val(Json::arrayValue);

    REQUIRE_NOTHROW(common::jsonToPathTree(tree, val));
    REQUIRE(val == common::pathTreeToJson(tree));
}

TEST_CASE("PathTreeJSON-ManualTreeToJsonWithRoundtrip") {
    PathTree tree;
    setupDummyTree(tree);

    REQUIRE_NOTHROW(common::pathTreeToJson(tree));
    auto json = common::pathTreeToJson(tree);
    INFO(json.toStyledString());
    REQUIRE_FALSE(json.size() == 0u);

    PathTree tree2;
    REQUIRE_NOTHROW(common::jsonToPathTree(tree2, json));
    REQUIRE(json == common::pathTreeToJson(tree2));
}

TEST_CASE("PathTreeJSON-SerializedTreeToJsonWithRoundtrip") {

    Json::Value val;
    Json::Reader reader;
    REQUIRE(reader.parse(getJSON(), val));

    PathTree tree;
    REQUIRE_NOTHROW(common::jsonToPathTree(tree, val));

    REQUIRE(common::pathTreeToJson(tree) == val);
}
