/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

/* Internal Includes */
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceStateC.h>

/* Library/third-party includes */
/* - none */

// Standard includes
#include <stdio.h>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("org.opengoggles.exampleclients.TrackerState");

    OSVR_ClientInterface lefthand = NULL;
    /* This is just one of the paths. You can also use:
     * /me/hands/right
     * /me/head
     */
    osvrClientGetInterface(ctx, "/me/hands/left", &lefthand);

    // Pretend that this is your application's mainloop.
    int i;
    for (i = 0; i < 1000000; ++i) {
        osvrClientUpdate(ctx);
        if (i % 100) {
            // Every so often let's read the tracker state.
            // Similar methods exist for all other stock report types.
            OSVR_PoseState state;
            OSVR_TimeValue timestamp;
            OSVR_ReturnCode ret =
                osvrGetPoseState(lefthand, &timestamp, &state);
            if (ret != OSVR_RETURN_SUCCESS) {
                printf("No pose state!\n");
            } else {
                printf("Got POSE state: Position = (%f, %f, %f), orientation = "
                       "(%f, %f, "
                       "%f, %f)\n",
                       report->pose.translation.data[0],
                       report->pose.translation.data[1],
                       report->pose.translation.data[2],
                       osvrQuatGetW(&(report->pose.rotation)),
                       osvrQuatGetX(&(report->pose.rotation)),
                       osvrQuatGetY(&(report->pose.rotation)),
                       osvrQuatGetZ(&(report->pose.rotation)));
            }
        }
    }

    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
    return 0;
}
