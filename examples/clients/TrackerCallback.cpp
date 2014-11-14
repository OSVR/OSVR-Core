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
#include <ogvr/ClientKit/InterfaceCallbackC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

void myTrackerCallback(void * /*userdata*/, OGVR_TimeValue timestamp,
                       const OGVR_PoseReport report) {
    std::cout << "Got report: Position = (" << report.pose.translation.data[0]
              << ", " << report.pose.translation.data[1] << ", "
              << report.pose.translation.data[2] << "), orientation = ("
              << ogvrQuatGetW(&(report.pose.rotation)) << ", ("
              << ogvrQuatGetX(&(report.pose.rotation)) << ", "
              << ogvrQuatGetY(&(report.pose.rotation)) << ", "
              << ogvrQuatGetZ(&(report.pose.rotation)) << ")" << std::endl;
}

int main() {
    OGVR_ClientContext ctx =
        ogvrClientInit("org.opengoggles.exampleclients.TrackerCallback");

    OGVR_ClientInterface lefthand = NULL;
    ogvrClientGetInterface(ctx, "/me/hands/left", &lefthand);
    ogvrRegisterPoseCallback(lefthand, &myTrackerCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        ogvrClientUpdate(ctx);
    }

    ogvrClientShutdown(ctx);
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
