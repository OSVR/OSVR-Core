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
#include <osvr/Routing/PathElementTools.h>
#include <osvr/Routing/PathElementTypes.h>
#include "IsType.h"

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using namespace osvr::routing;

TEST(PathElement, getTypeName) {
    ASSERT_STREQ(elements::getTypeName<elements::NullElement>(), "NullElement");
}

TEST(PathElement, isElementType) {
    ASSERT_TRUE(isElementType<elements::NullElement>(elements::NullElement()));
    ASSERT_FALSE(
        isElementType<elements::DeviceElement>(elements::NullElement()));
}
