/** @file
    @brief Test Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Routing/PathTreeFull.h>
#include <osvr/Routing/PathElementTypes.h>
#include <osvr/Routing/PathNode.h>
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
    PathNode *result = NULL;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/")) << "Get the root.";
    ASSERT_TRUE(result->isRoot());
}

TEST(PathTree, getPathBadInput) {
    PathTree tree;
    ASSERT_THROW(tree.getNodeByPath(""), std::runtime_error)
        << "Empty string not acceptable as path";
    ASSERT_THROW(tree.getNodeByPath("test"), std::runtime_error)
        << "Missing leading slash not acceptable";
}

TEST(PathTree, getPathSingleLevel) {
    PathTree tree;
    PathNode *result = NULL;
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
    PathNode *test2 = NULL;
    ASSERT_NO_THROW(test2 = &tree.getNodeByPath("/test1/test2"))
        << "Get a node two new levels in";
    // Check test2
    ASSERT_EQ(test2->getName(), "test2");
    ASSERT_FALSE(test2->hasChildren()) << "Make sure it has no children.";
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test2)) << "Check type";
    ASSERT_FALSE(!test2->getParent()) << "Make sure it has a parent.";

    // Check test1
    PathNodePtr test1 = test2->getParent();
    ASSERT_EQ(test1->getName(), "test1");
    ASSERT_TRUE(isNodeType<elements::NullElement>(*test1));
    ASSERT_FALSE(!test1->getParent()) << "Make sure it has a parent.";
    PathNodePtr root = test1->getParent();

    ASSERT_TRUE(root->isRoot());
    ASSERT_EQ(tree.getNodeByPath("/"), *root)
        << "Root identity should be preserved";
    ASSERT_EQ(tree.getNodeByPath("/test1/test2"), *test2)
        << "Identity should be preserved";
}
