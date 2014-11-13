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
#include <ogvr/ClientKit/ContextC.h>
#include <ogvr/ClientKit/InterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    OGVR_ClientContext ctx =
        ogvrClientInit("org.opengoggles.exampleclients.TrackerCallback");

    OGVR_ClientInterface lefthand = NULL;
    ogvrClientGetInterface(ctx, "/me/hands/left", &lefthand);

    ogvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
