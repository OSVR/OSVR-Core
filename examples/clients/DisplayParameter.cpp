/** @file
    @brief Implementation

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
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/Parameters.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("org.opengoggles.exampleclients.DisplayParameter");

    // Using the C++ (header-only) wrapper here, can do the same effective thing
    // with the bare C API
    std::string displayDescription =
        osvr::clientkit::getStringParameter(ctx, "/display");

    std::cout << "Got value of /display:\n" << displayDescription << std::endl;

    osvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
