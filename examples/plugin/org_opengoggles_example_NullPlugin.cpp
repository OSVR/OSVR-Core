/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OGVR_PLUGIN(org_opengoggles_example_NullPlugin) {
    std::cout << "In plugin EP!" << std::endl;
    return OGVR_RETURN_SUCCESS;
}