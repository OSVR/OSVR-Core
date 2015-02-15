/** @file
    @brief Implementation

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
#include <osvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OSVR_PLUGIN(org_opengoggles_example_NullPlugin) {
    std::cout << "In plugin EP!" << std::endl;
    return OSVR_RETURN_SUCCESS;
}