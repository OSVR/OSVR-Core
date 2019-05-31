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
#include "IsType.h"
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/RoutingExceptions.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
// - none

using std::string;
using namespace osvr::common;
TEST_CASE("PathTree-create") { REQUIRE_NOTHROW(PathTree()); }

TEST_CASE("PathTree-getPathRoot") {
    PathTree tree;
    PathNode *result = nullptr;
    REQUIRE_NOTHROW(result = &tree.getNodeByPath("/"));
    REQUIRE(result->isRoot());
    REQUIRE(result->getParent() == nullptr);
}

TEST_CASE("PathTree-getPathBadInput") {
    PathTree tree;
    REQUIRE_THROWS_AS(tree.getNodeByPath(""), exceptions::EmptyPath);
    {
        INFO("Missing leading slash not acceptable");
        REQUIRE_THROWS_AS(tree.getNodeByPath("test"),
                          exceptions::PathNotAbsolute);
    }
    REQUIRE_THROWS_AS(tree.getNodeByPath("//test"),
                      exceptions::EmptyPathComponent);
    SECTION("Empty component not OK!") {
        REQUIRE_THROWS_AS(tree.getNodeByPath("/asdf//test"),
                          exceptions::EmptyPathComponent);
        REQUIRE_THROWS_AS(tree.getNodeByPath("/asdf//"),
                          exceptions::EmptyPathComponent);
    }
}

TEST_CASE("PathTree-getPathSingleLevel") {
    PathTree tree;
    PathNode *result = nullptr;
    {
        INFO("Get a new node just a single level in");
        REQUIRE_NOTHROW(result = &tree.getNodeByPath("/test"));
    }
    {
        INFO("Check the type of the new node.");
        REQUIRE(isNodeType<elements::NullElement>(*result));
    }
    {
        INFO("Check the name of the new node");
        REQUIRE(result->getName() == "test");
    }

    {
        INFO("Get the same node again.");
        REQUIRE_NOTHROW(tree.getNodeByPath("/test"));
    }
    {
        INFO("Ensure we're actually getting the same identity.");
        REQUIRE(tree.getNodeByPath("/test") == *result);
    }
    {
        INFO("Ensure trailing slashes don't matter.");
        REQUIRE(tree.getNodeByPath("/test/") == *result);
    }
}

TEST_CASE("PathTree-getPathTwoLevel") {
    PathTree tree;
    PathNode *test2 = nullptr;
    {
        INFO("Get a node two new levels in");
        REQUIRE_NOTHROW(test2 = &tree.getNodeByPath("/test1/test2"));
    }
    // Check test2
    SECTION("Check test2 - node two new levels in") {
        REQUIRE(test2->getName() == "test2");
        REQUIRE_FALSE(test2->hasChildren());
        REQUIRE(isNodeType<elements::NullElement>(*test2));
        REQUIRE_FALSE(test2->getParent() == nullptr);
    }

    // Check test1

    auto test1 = test2->getParent();
    SECTION("Check test1, parent of test2") {
        REQUIRE(test1->getName() == "test1");
        REQUIRE(isNodeType<elements::NullElement>(*test1));
        REQUIRE_FALSE(test1->getParent() == nullptr);
    }
    auto root = test1->getParent();

    SECTION("Verify root preservation") {
        REQUIRE(root->isRoot());
        REQUIRE(tree.getNodeByPath("/") == *root);
    }
    {
        INFO("Identity should be preserved");
        REQUIRE(tree.getNodeByPath("/test1/test2") == *test2);
    }
}
