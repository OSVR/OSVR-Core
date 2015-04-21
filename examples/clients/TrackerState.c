/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2014 Sensics, Inc.
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
#include <osvr/ClientKit/InterfaceStateC.h>

/* Library/third-party includes */
/* - none */

// Standard includes
#include <stdio.h>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.exampleclients.TrackerState", 0);

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
                       state.translation.data[0], state.translation.data[1],
                       state.translation.data[2],
                       osvrQuatGetW(&(state.rotation)),
                       osvrQuatGetX(&(state.rotation)),
                       osvrQuatGetY(&(state.rotation)),
                       osvrQuatGetZ(&(state.rotation)));
            }
        }
    }

    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
    return 0;
}
