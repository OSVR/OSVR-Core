/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

// Internal Includes
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/SkeletonC.h>

// Library/third-party includes

// Standard includes
#include <stdio.h>

void mySkeletonCallback(void *userdata, const OSVR_TimeValue *timestamp, const OSVR_SkeletonReport *report) {
    OSVR_ReturnCode rc;
    OSVR_Skeleton skel = (OSVR_Skeleton)userdata;
    printf("Got report: channel is %d, dataAvailable is %s\n", report->sensor, report->state.dataAvailable ? "true" : "false");
    if (report->state.dataAvailable) {
        OSVR_SkeletonJointCount numJoints = 0;
        rc = osvrClientGetSkeletonNumJoints(skel, &numJoints);
        if (rc == OSVR_RETURN_FAILURE) {
            printf("call to osvrClientGetSkeletonNumJoints failed");
        }

        OSVR_SkeletonBoneCount numBones = 0;
        rc = osvrClientGetSkeletonNumBones(skel, &numBones);
        if (rc == OSVR_RETURN_FAILURE) {
            printf("call to osvrClientGetSkeletonNumJoints failed");
        }

        printf("numJoints: %d, numBones: %d", numJoints, numBones);
    }
}

#define CHECK_RC(rc) if(rc == OSVR_RETURN_FAILURE) { printf("OSVR call failed"); if(skel) { osvrClientFreeSkeleton(skel); } if(ctx) { osvrClientShutdown(ctx); } return -1; }

int main() {
    OSVR_ReturnCode rc;
    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.exampleclients.SkeletonCallback", 0);

    OSVR_ClientInterface leftHand = NULL;
    OSVR_Skeleton skel = NULL;

    rc = osvrClientGetInterface(
        ctx, "/com_osvr_example_Skeleton/Skeleton/skeleton/0", &leftHand);
    CHECK_RC(rc);

    while (osvrClientCheckStatus(ctx) == OSVR_RETURN_FAILURE) {
        osvrClientUpdate(ctx);
    }

    rc = osvrClientGetSkeleton(ctx, leftHand, &skel);
    CHECK_RC(rc);

    rc = osvrRegisterSkeletonCallback(leftHand, mySkeletonCallback, skel);
    CHECK_RC(rc);

    // Pretend that this is your application's mainloop.
    while (1) {
        rc = osvrClientUpdate(ctx);
        CHECK_RC(rc);
        // get some skeleton Reports
    }

    if (skel) {
        osvrClientFreeSkeleton(skel);
    }

    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
    return 0;
}
