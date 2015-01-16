/** @file
    @brief Test Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Routing/PathTreeFull.h>
#include <osvr/Routing/PathNode.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using namespace osvr::routing;

TEST(PathNode, getFullPath) {
    PathTree tree;
    ASSERT_EQ(getFullPath(tree.getNodeByPath("/")), "/") << "Root";
    ASSERT_EQ(getFullPath(tree.getNodeByPath("/test")), "/test")
        << "First level";
    ASSERT_EQ(
        getFullPath(tree.getNodeByPath("/org_opengoggles_sample/MyDevice")),
        "/org_opengoggles_sample/MyDevice")
        << "Second level";
}

TEST(PathNode, getFullPathNormalizes) {
    PathTree tree;
    ASSERT_EQ(
        getFullPath(tree.getNodeByPath("/org_opengoggles_sample/MyDevice/")),
        "/org_opengoggles_sample/MyDevice");
}