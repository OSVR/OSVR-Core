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
#include <osvr/ClientKit/InterfaceCallbackC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

void myAnalogCallback(void * /*userdata*/, const OSVR_TimeValue * /*timestamp*/,
                      const OSVR_AnalogReport *report) {
    std::cout << "Got report: channel is " << report->state << std::endl;
}

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("org.opengoggles.exampleclients.AnalogCallback");

    OSVR_ClientInterface analogTrigger = NULL;
    // This is just one of the paths: specifically, the Hydra's left
    // controller's analog trigger. More are in the docs and/or listed on
    // startup
    osvrClientGetInterface(ctx, "/controller/left/trigger", &analogTrigger);

    osvrRegisterAnalogCallback(analogTrigger, &myAnalogCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        osvrClientUpdate(ctx);
    }

    osvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
