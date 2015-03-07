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
#include <osvr/Routing/PathTreeFull.h>
#include <osvr/Routing/PathElementTypes.h>
#include <osvr/Routing/PathNode.h>
#include <osvr/Routing/Exceptions.h>
#include "IsType.h"

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using std::string;
using namespace osvr::routing;
TEST(PathTree, create) { ASSERT_NO_THROW(PathTree()); }

TEST(PathTree, getPathRoot) {
    PathTree tree;
    PathNode *result = nullptr;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/")) << "Get the root.";
    ASSERT_TRUE(result->isRoot());
    ASSERT_EQ(result->getParent(), nullptr);
}

TEST(PathTree, getPathBadInput) {
    PathTree tree;
    ASSERT_THROW(tree.getNodeByPath(""), exceptions::EmptyPath)
        << "Empty string not acceptable as path";
    ASSERT_THROW(tree.getNodeByPath("test"), exceptions::PathNotAbsolute)
        << "Missing leading slash not acceptable";
    ASSERT_THROW(tree.getNodeByPath("//test"), exceptions::EmptyPathComponent)
        << "Empty component not OK!";
    ASSERT_THROW(tree.getNodeByPath("/asdf//test"),
                 exceptions::EmptyPathComponent)
        << "Empty component not OK!";
    ASSERT_THROW(tree.getNodeByPath("/asdf//"), exceptions::EmptyPathComponent)
        << "Empty component not OK!";
}

TEST(PathTree, getPathSingleLevel) {
    PathTree tree;
    PathNode *result = nullptr;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/test"))
        << "Get a new node just a single level in";
    ASSERT_TRUE(isNodeType<elements::NullElement>(*result))
        << "Check the type of the new node.";
    ASSERT_EQ(result->getName(), "test") << "Check the name of the new node";

    ASSERT_NO_THROW(tree.getNodeByPath("/test")) << "Get the same node again.";
    ASSERT_EQ(tree.getNodeByPath("/test"), *result)
        << "Ensure we're actually getting the same identity.";
    ASSERT_EQ(tree.getNodeByPath("/test/"), *result)
        << "Ensure trailing slashes don't matter.";
}

TEST(PathTree, getPathTwoLevel) {
    PathTree tree;
    PathNode *test2 = nullptr;
    ASSERT_NO_THROW(test2 = &tree.getNodeByPath("/test1/test2"))
        << "Get a node two new levels in";
    // Check test2
    ASSERT_EQ(test2->getName(), "test2");
    ASSERT_FALSE(test2->hasChildren()) << "Make sure it has no children.";
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test2)) << "Check type";
    ASSERT_NE(test2->getParent(), nullptr) << "Make sure it has a parent.";

    // Check test1
    auto test1 = test2->getParent();
    ASSERT_EQ(test1->getName(), "test1");
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test1));
    ASSERT_NE(test1->getParent(), nullptr) << "Make sure it has a parent.";
    auto root = test1->getParent();

    ASSERT_TRUE(root->isRoot());
    ASSERT_EQ(tree.getNodeByPath("/"), *root)
        << "Root identity should be preserved";
    ASSERT_EQ(tree.getNodeByPath("/test1/test2"), *test2)
        << "Identity should be preserved";
}
