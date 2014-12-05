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

void myButtonCallback(void * /*userdata*/, const OSVR_TimeValue * /*timestamp*/,
                      const OSVR_ButtonReport *report) {
    std::cout << "Got report: button is "
              << (report->state ? "pressed" : "released") << std::endl;
}

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("org.opengoggles.exampleclients.ButtonCallback");

    OSVR_ClientInterface button1 = NULL;
    // This is just one of the paths: specifically, the Hydra's left
    // controller's button labelled "1". More are in the docs and/or listed on
    // startup
    osvrClientGetInterface(ctx, "/controller/left/1", &button1);

    osvrRegisterButtonCallback(button1, &myButtonCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        osvrClientUpdate(ctx);
    }

    osvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
