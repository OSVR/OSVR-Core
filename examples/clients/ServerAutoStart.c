/** @file
    @brief Implementation
    
    @date 2016
    
    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

/* Internal Includes */
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/ServerAutoStartC.h>

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
    /* This call attempts to auto-start the OSVR server in a platform-specific way. 
     * This only needs to be called once for a given process, though the function is idempotent.
     * There is no way to determine if the server started successfully except to check that
     * the OSVR_ClientContext successfully connected to it.
     */
    osvrClientAttemptServerAutoStart();

    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.exampleclients.TrackerCallback", 0);

    /* This is just one of the paths. You can also use:
     * /me/hands/right
     * /me/head
     */
    OSVR_ClientInterface lefthand = NULL;
    osvrClientGetInterface(ctx, "/me/hands/left", &lefthand);

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
        osvrClientUpdate(ctx);
    }

    /* Call this once per process. This cleans up any platform-specific resources related
     * to the server auto-start functionality. While this function is idempotent, calling it may
     * shut down the server on some platforms.
     */
    osvrClientShutdown(ctx);

    printf("Library shut down, exiting.\n");
    return 0;
}
