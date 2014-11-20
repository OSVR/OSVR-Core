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
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <string>

using std::string;
using osvr::util::TreeNode;
using osvr::util::TreeNodePointer;

typedef TreeNode<int> IntTree;
typedef TreeNodePointer<int>::type IntTreePtr;

TEST(TreeNode, ConstructRoot) {
    ASSERT_NO_THROW((IntTree::createRoot()));
}

TEST(TreeNode, RootInvariants) {
    IntTreePtr tree;
    ASSERT_NO_THROW((tree = IntTree::createRoot()));
    ASSERT_TRUE((tree->getName().empty()));
}
