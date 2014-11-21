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
#include <osvr/Routing/PathElementTools.h>
#include <osvr/Routing/PathElementTypes.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using std::string;
using namespace osvr::routing;
// using osvr::routing::PathTree;
// namespace elements = osvr::routing::elements;

TEST(PathTree, create) { ASSERT_NO_THROW(PathTree()); }

TEST(PathElement, getTypeName) {
    ASSERT_STREQ(elements::getTypeName<elements::NullElement>(), "NullElement");
}

TEST(PathTree, getPath) {
    PathTree tree;
    ASSERT_NO_THROW(tree.getNodeByPath("/")) << "Get the root.";

    {
        PathNode *result = NULL;
        ASSERT_NO_THROW(result = &tree.getNodeByPath("/test"))
            << "Get a new node just a single level in";
        ASSERT_STREQ(getTypeName(*result), "NullElement")
            << "Check the type of the new node.";
        ASSERT_EQ(result->getName(), "test")
            << "Check the name of the new node";

        ASSERT_NO_THROW(tree.getNodeByPath("/test"))
            << "Get the same node again.";
        ASSERT_EQ(tree.getNodeByPath("/test"), *result)
            << "Ensure we're actually getting the same identity.";
    }
    {
        PathNode *test2 = NULL;
        ASSERT_NO_THROW(test2 = &tree.getNodeByPath("/test1/test2"))
            << "Get a node two new levels in";
        ASSERT_EQ(test2->getName(), "test2");
        ASSERT_TRUE(test2->getParent().get());
        PathNodePtr test1 = test2->getParent();
    }
}