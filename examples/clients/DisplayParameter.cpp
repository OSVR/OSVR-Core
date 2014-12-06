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
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Parameters.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    osvr::clientkit::ClientContext context(
        "org.opengoggles.exampleclients.DisplayParameter");

    std::string displayDescription = context.getStringParameter("/display");

    std::cout << "Got value of /display:\n" << displayDescription << std::endl;

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
