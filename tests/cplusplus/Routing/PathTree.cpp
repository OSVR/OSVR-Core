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

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using std::string;
using osvr::routing::PathTree;

TEST(PathTree, create) {
    ASSERT_NO_THROW(PathTree());

    ASSERT_NO_THROW(PathTree().getRoot());
    ASSERT_TRUE(PathTree().getRoot().isRoot());
    ASSERT_EQ(PathTree().getRoot().numChildren(), 0);
}
