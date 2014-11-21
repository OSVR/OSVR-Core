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

TEST(PathTree, getPath) {
    PathTree tree;
    ASSERT_NO_THROW(tree.getNodeByPath("/test"));
    ASSERT_STREQ(getTypeName(tree.getNodeByPath("/test")), "NullElement");
    ASSERT_STREQ(elements::getTypeName<elements::NullElement>(), "NullElement");
    ASSERT_EQ(tree.getNodeByPath("/test").getName(), "test");

    PathNode *result = NULL;
    ASSERT_NO_THROW(result = &tree.getNodeByPath("/test1/test2"));
    ASSERT_EQ(result->getName(), "test2");
}