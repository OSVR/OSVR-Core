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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/SkeletonC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetSkeletonBoneId(
    OSVR_ClientInterface skeletonIface, const char *boneName,
    OSVR_SkeletonBoneCount *boneId){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonBoneState(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount boneId,
    OSVR_SkeletonBoneState *state){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonJointId(
    OSVR_ClientInterface skeletonIface, const char *jointName,
    OSVR_SkeletonJointCount *jointId){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonJointState(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount jointId,
    OSVR_SkeletonJointState *state){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonNumBones(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount *numBones){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonNumJoints(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount *numJoints){

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetChildJointId(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount jointId,
    OSVR_SkeletonJointCount childNum, OSVR_SkeletonJointCount *childJointId){

    return OSVR_RETURN_SUCCESS;
}

