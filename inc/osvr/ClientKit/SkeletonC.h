/** @file
    @brief Header

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

#ifndef INCLUDED_SkeletonC_h_GUID_999D5F46_F1C3_41B5_0EEC_9746ACBA9270
#define INCLUDED_SkeletonC_h_GUID_999D5F46_F1C3_41B5_0EEC_9746ACBA9270

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/StringIds.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief Convert a given boneName to the boneId
@param skeletonIface skeleton interface
@param boneName string containing boneName
@param boneId The bone id for provided boneName. If provided
bone name does not exist, the boneId will be unchanged. Refer
to OSVR_ReturnCode return value to make sure it exists.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonBoneId(
    OSVR_ClientInterface skeletonIface, const char *boneName,
    OSVR_SkeletonBoneCount *boneId);

/** @brief Get the length of a string parameter associated with the given
boneId.
@param skeletonIface Skeleton Interface
@param boneId associated id of the bone
@param[out] len The length of the string value, including null terminator. 0
if the parameter does not exist or is not a string.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonStringBoneNameLength(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount boneId,
    size_t *len);

/** @brief Convert the boneId to the bone name
@param skeletonIface skeleton interface
@param boneId  An id that corresponds to this bone
@param [in, out] boneName A buffer that you allocate of appropriate size.
Must be at least the length returned by
osvrClientGetSkeletonStringBoneNameLength.
Will contain the null-terminated string name of the boneId.
@param len The length of the buffer you're providing. If the buffer is too
short, an error is returned and the buffer is unchanged.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonBoneName(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount boneId,
    char *boneName, size_t len);

/** @brief Get skeleton bone state for given boneId
@param skeletonIface skeleton interface
@param boneId The bone id for provided boneName.
@param state bone state containing boneId and tracker report
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonBoneState(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount boneId,
    OSVR_SkeletonBoneState *state);

/** @brief Convert a given jointName to the jointId
@param skeletonIface skeleton interface
@param jointName string containing boneName
@param jointId The joint id for provided boneName. If provided
bone name does not exist, the boneId will be unchanged. Refer
to OSVR_ReturnCode return value to make sure it exists.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonJointId(
    OSVR_ClientInterface skeletonIface, const char *jointName,
    OSVR_SkeletonJointCount *jointId);

/** @brief Get the length of a string parameter associated with the given
jointId.
@param skeletonIface Skeleton Interface
@param jointId associated id of the joint
@param[out] len The length of the string value, including null terminator. 0
if the parameter does not exist or is not a string.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetSkeletonStringJointNameLength(OSVR_ClientInterface skeletonIface,
                                           OSVR_SkeletonJointCount jointId,
                                           size_t *len);

/** @brief Convert the jointId to the joint name
@param skeletonIface skeleton interface
@param jointId  An id that corresponds to this joint
@param [in, out] jointName A buffer that you allocate of appropriate size.
Must be at least the length returned by
osvrClientGetSkeletonStringJointNameLength.
Will contain the null-terminated string name of the boneId.
@param len The length of the buffer you're providing. If the buffer is too
short, an error is returned and the buffer is unchanged.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonJointName(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount boneId,
    char *jointName, size_t len);

/** @brief Get skeleton joint state for given joint Id
@param skeletonIface skeleton interface
@param boneId The joint Id corresponding to a joint in current skeleton
@param state joint state containing jointId and tracker report
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonJointState(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount jointId,
    OSVR_SkeletonJointState *state);

/** @brief Get the number of bones available for a given skeleton
@param skeletonIface skeleton interface
@param numBones on return contains a number of bones in current skeleton
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonNumBones(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonBoneCount *numBones);

/** @brief Get the number of joints available for a given skeleton
@param skeletonIface skeleton interface
@param numJoints on return contains a number of joints in current skeleton
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonNumJoints(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount *numJoints);

/** @brief For a given joint Id, get the Id of its child
@param skeletonIface skeleton interface
@param jointId parent joint Id
@param childNum
@param childJointId on return contains the joint Id of the child specified by
parent joint Id
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetChildJointId(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount jointId,
    OSVR_SkeletonJointCount childNum, OSVR_SkeletonJointCount *childJointId);

OSVR_EXTERN_C_END

#endif // INCLUDED_SkeletonC_h_GUID_999D5F46_F1C3_41B5_0EEC_9746ACBA9270
