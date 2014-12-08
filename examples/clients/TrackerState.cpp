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
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceStateC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("org.opengoggles.exampleclients.TrackerState");

    OSVR_ClientInterface lefthand = NULL;
    // This is just one of the paths. You can also use:
    // /me/hands/right
    // /me/head
    osvrClientGetInterface(ctx, "/me/hands/left", &lefthand);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        osvrClientUpdate(ctx);
        if (i % 100) {
            // Every so often let's read the tracker state.
            // Similar methods exist for all other stock report types.
            OSVR_PoseState state;
            OSVR_TimeValue timestamp;
            OSVR_ReturnCode ret =
                osvrGetPoseState(lefthand, &timestamp, &state);
            if (ret != OSVR_RETURN_SUCCESS) {
                std::cout << "No pose state!" << std::endl;
            } else {
                std::cout << "Got POSE state: Position = ("
                          << state.translation.data[0] << ", "
                          << state.translation.data[1] << ", "
                          << state.translation.data[2] << "), orientation = ("
                          << osvrQuatGetW(&(state.rotation)) << ", ("
                          << osvrQuatGetX(&(state.rotation)) << ", "
                          << osvrQuatGetY(&(state.rotation)) << ", "
                          << osvrQuatGetZ(&(state.rotation)) << ")"
                          << std::endl;
            }
        }
    }

    osvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
