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

#define NAME_BUFFER_SIZE 10000

void mySkeletonCallback(void *userdata, const OSVR_TimeValue *timestamp, const OSVR_SkeletonReport *report) {
    OSVR_ReturnCode rc;
    OSVR_Skeleton skel = (OSVR_Skeleton)userdata;
    printf("-----------\n");
    printf("\tGot report: channel is %d, dataAvailable is %s\n", report->sensor, report->state.dataAvailable ? "true" : "false");
    if (report->state.dataAvailable) {
        OSVR_SkeletonJointCount numJoints = 0;
        rc = osvrClientGetSkeletonNumJoints(skel, &numJoints);
        if (rc == OSVR_RETURN_FAILURE) {
            printf("\tcall to osvrClientGetSkeletonNumJoints failed\n");
        }

        OSVR_SkeletonBoneCount numBones = 0;
        rc = osvrClientGetSkeletonNumBones(skel, &numBones);
        if (rc == OSVR_RETURN_FAILURE) {
            printf("\tcall to osvrClientGetSkeletonNumJoints failed\n");
        }

        printf("\tnumJoints: %d, numBones: %d\n", numJoints, numBones);

        for (OSVR_SkeletonJointCount joint = 0; joint < numJoints; joint++) {
            uint32_t len = 0;
            OSVR_SkeletonJointCount jointId = 0;
            rc = osvrClientGetSkeletonAvailableJointId(skel, joint, &jointId);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tcouldn't get the joint id for joint index %d\n", joint);
                continue;
            }

            rc = osvrClientGetSkeletonStringJointNameLength(skel, jointId, &len);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tjoint %d: call to osvrClientGetSkeletonStringJointNameLength failed", joint);
                continue;
            }

            char jointName[NAME_BUFFER_SIZE];
            rc = osvrClientGetSkeletonJointName(skel, jointId, jointName, NAME_BUFFER_SIZE);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tjoint %d: osvrClientGetSkeletonJointName call failed. Perhaps the joint name is greater than our buffer size?\n", joint);
                continue;
            }

            OSVR_SkeletonJointState jointState = { 0 };
            rc = osvrClientGetSkeletonJointState(skel, jointId, &jointState);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tjoint %d: osvrClientGetSkeletonJointState call failed.\n", joint);
                continue;
            }

            printf("\tjoint %d: name: %s,\n\t\ttranslation: (x: %f, y: %f, z: %f)\n\t\torientation: (x: %f, y: %f, z: %f, w: %f)\n",
                joint, jointName,
                osvrVec3GetX(&jointState.pose.translation),
                osvrVec3GetY(&jointState.pose.translation),
                osvrVec3GetZ(&jointState.pose.translation),
                osvrQuatGetX(&jointState.pose.rotation),
                osvrQuatGetY(&jointState.pose.rotation),
                osvrQuatGetZ(&jointState.pose.rotation),
                osvrQuatGetW(&jointState.pose.rotation));
        }

        for (OSVR_SkeletonBoneCount bone = 0; bone < numBones; bone++) {
            uint32_t len = 0;
            OSVR_SkeletonBoneCount boneId = 0;
            rc = osvrClientGetSkeletonAvailableBoneId(skel, bone, &boneId);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tcouldn't get the bone id for bone index %d\n", bone);
                continue;
            }

            rc = osvrClientGetSkeletonStringBoneNameLength(skel, boneId, &len);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tbone %d: call to osvrClientGetSkeletonStringBoneNameLength failed", bone);
                continue;
            }

            char boneName[NAME_BUFFER_SIZE];
            rc = osvrClientGetSkeletonBoneName(skel, boneId, boneName, NAME_BUFFER_SIZE);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tbone %d: osvrClientGetSkeletonBoneName call failed. Perhaps the bone name is greater than our buffer size?\n", bone);
                continue;
            }

            OSVR_SkeletonBoneState boneState = { 0 };
            rc = osvrClientGetSkeletonBoneState(skel, boneId, &boneState);
            if (rc == OSVR_RETURN_FAILURE) {
                printf("\tbone %d: osvrClientGetSkeletonBoneState call failed.\n", bone);
                continue;
            }

            printf("\tbone %d: name: %s,\n\t\ttranslation: (x: %f, y: %f, z: %f)\n\t\torientation: (x: %f, y: %f, z: %f, w: %f)\n",
                bone, boneName,
                osvrVec3GetX(&boneState.pose.translation),
                osvrVec3GetY(&boneState.pose.translation),
                osvrVec3GetZ(&boneState.pose.translation),
                osvrQuatGetX(&boneState.pose.rotation),
                osvrQuatGetY(&boneState.pose.rotation),
                osvrQuatGetZ(&boneState.pose.rotation),
                osvrQuatGetW(&boneState.pose.rotation));
        }

        OSVR_SkeletonJointCount leftWristJointId = 0;
        rc = osvrClientGetSkeletonJointId(skel, "leftWrist", &leftWristJointId);
        printf("\tasking for 'leftWrist' joint id by name: %s, jointId = %d\n",
            rc == OSVR_RETURN_SUCCESS ? "suceeded" : "failed", leftWristJointId);
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

    while (!skel) {
        osvrClientUpdate(ctx);
        rc = osvrClientGetSkeleton(ctx, leftHand, &skel);
        if (rc == OSVR_RETURN_SUCCESS) {
            break;
        }
    }

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
