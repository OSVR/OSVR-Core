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
#include <osvr/ClientKit/Interface.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

void myTrackerCallback(void * /*userdata*/,
                       const OSVR_TimeValue * /*timestamp*/,
                       const OSVR_PoseReport *report) {
    std::cout << "Got POSE report: Position = ("
              << report->pose.translation.data[0] << ", "
              << report->pose.translation.data[1] << ", "
              << report->pose.translation.data[2] << "), orientation = ("
              << osvrQuatGetW(&(report->pose.rotation)) << ", ("
              << osvrQuatGetX(&(report->pose.rotation)) << ", "
              << osvrQuatGetY(&(report->pose.rotation)) << ", "
              << osvrQuatGetZ(&(report->pose.rotation)) << ")" << std::endl;
}

void myOrientationCallback(void * /*userdata*/,
                           const OSVR_TimeValue * /*timestamp*/,
                           const OSVR_OrientationReport *report) {
    std::cout << "Got ORIENTATION report: Orientation = ("
              << osvrQuatGetW(&(report->rotation)) << ", ("
              << osvrQuatGetX(&(report->rotation)) << ", "
              << osvrQuatGetY(&(report->rotation)) << ", "
              << osvrQuatGetZ(&(report->rotation)) << ")" << std::endl;
}

void myPositionCallback(void * /*userdata*/,
                        const OSVR_TimeValue * /*timestamp*/,
                        const OSVR_PositionReport *report) {
    std::cout << "Got POSITION report: Position = (" << report->xyz.data[0]
              << ", " << report->xyz.data[1] << ", " << report->xyz.data[2]
              << ")" << std::endl;
}

int main() {
    osvr::clientkit::ClientContext context(
        "org.opengoggles.exampleclients.TrackerCallback");

    // This is just one of the paths. You can also use:
    // /me/hands/right
    // /me/head
    osvr::clientkit::Interface lefthand =
        context.getInterface("/me/hands/left");

    // The coordinate system is right-handed, with X to the right, Y up, and Z
    // near.
    lefthand.registerCallback(&myTrackerCallback, NULL);

    /// If you just want orientation
    lefthand.registerCallback(&myOrientationCallback, NULL);

    /// or position
    lefthand.registerCallback(&myPositionCallback, NULL);

    // Pretend that this is your application's mainloop.
    for (int i = 0; i < 1000000; ++i) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
