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
#include <osvr/ClientKit/InterfaceCallbackC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdio.h>

void myTrackerCallback(void *userdata, const OSVR_TimeValue *timestamp,
                       const OSVR_PoseReport *report) {
    printf("Got POSE report: Position = (%f, %f, %f), orientation = (%f, %f, "
           "%f, %f)\n",
           report->pose.translation.data[0], report->pose.translation.data[1],
           report->pose.translation.data[2],
           osvrQuatGetW(&(report->pose.rotation)),
           osvrQuatGetX(&(report->pose.rotation)),
           osvrQuatGetY(&(report->pose.rotation)),
           osvrQuatGetZ(&(report->pose.rotation)));
}

void myOrientationCallback(void *userdata, const OSVR_TimeValue *timestamp,
                           const OSVR_OrientationReport *report) {
    printf("Got ORIENTATION report: Orientation = (%f, %f, %f, %f)\n",
           osvrQuatGetW(&(report->rotation)), osvrQuatGetX(&(report->rotation)),
           osvrQuatGetY(&(report->rotation)),
           osvrQuatGetZ(&(report->rotation)));
}

void myPositionCallback(void *userdata, const OSVR_TimeValue *timestamp,
                        const OSVR_PositionReport *report) {
    printf("Got POSITION report: Position = (%f, %f, %f)\n",
           report->xyz.data[0], report->xyz.data[1], report->xyz.data[2]);
}

int main() {
    OSVR_ClientContext context =
        osvrClientInit("org.opengoggles.exampleclients.TrackerCallback", 0);

    /* This is just one of the paths. You can also use:
     * /me/hands/right
     * /me/head
     */
    OSVR_ClientInterface lefthand = NULL;
    osvrClientGetInterface(context, "/me/hands/left", &lefthand);

    /* The coordinate system is right-handed, with X to the right, Y up, and Z
     * near.
     */
    osvrRegisterPoseCallback(lefthand, &myTrackerCallback, NULL);

    /* If you just want orientation */
    osvrRegisterOrientationCallback(lefthand, &myOrientationCallback, NULL);

    /* or position */
    osvrRegisterPositionCallback(lefthand, &myPositionCallback, NULL);

    /* Pretend that this is your application's mainloop. */
    int i;
    for (i = 0; i < 1000000; ++i) {
        osvrClientUpdate(context);
    }

    printf("Library shut down, exiting.\n");
    return 0;
}
