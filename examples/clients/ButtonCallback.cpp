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
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>

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
    osvr::clientkit::ClientContext context(
        "org.opengoggles.exampleclients.ButtonCallback");

    // This is just one of the paths: specifically, the Hydra's left
    // controller's button labelled "1". More are in the docs and/or listed on
    // startup
    osvr::clientkit::Interface button1 =
        context.getInterface("/controller/left/1");

    button1.registerCallback(&myButtonCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
