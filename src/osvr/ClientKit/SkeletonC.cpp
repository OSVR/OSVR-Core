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
#include <osvr/Client/SkeletonConfig.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/assert.hpp>

// disables warning about potentially unsafe string copy function usage.
#pragma warning( disable : 4996 )

struct OSVR_SkeletonObject {
    OSVR_SkeletonObject(OSVR_ClientContext ctx,
                        OSVR_ClientInterface skeletonIface)
        : m_ctx(ctx), m_iface(skeletonIface),
          cfg(osvr::client::SkeletonConfigFactory::create(ctx, skeletonIface)) {
        /// OSVR_DEV_VERBOSE("Created OSVR_SkeletonObject!");
    }
    ~OSVR_SkeletonObject() {
        /// OSVR_DEV_VERBOSE("Destroyed OSVR_SkeletonObject!");
    }

    OSVR_ClientContext m_ctx = nullptr;
    OSVR_ClientInterface m_iface = nullptr;
    osvr::client::SkeletonConfigPtr cfg = nullptr;
};

#define OSVR_VALIDATE_OUTPUT_PTR(X, DESC)                                      \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == X) {                                                        \
        OSVR_DEV_VERBOSE("Passed a null pointer for output parameter " #X      \
                         ", " DESC "!");                                       \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetSkeleton(OSVR_ClientContext ctx,
                      OSVR_ClientInterface skeletonIface, OSVR_Skeleton *skel) {

    OSVR_VALIDATE_OUTPUT_PTR(skel, "skeleton");
    if (ctx == nullptr) {
        OSVR_DEV_VERBOSE("Pass a null client context!");
        *skel = nullptr;
        return OSVR_RETURN_FAILURE;
    }
    std::shared_ptr<OSVR_SkeletonObject> skeleton;

    try {
        skeleton = std::make_shared<OSVR_SkeletonObject>(ctx, skeletonIface);
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error creating skeleton object : constructor threw exception :"
            << e.what());
        return OSVR_RETURN_FAILURE;
    }
    if (!skeleton) {
        /// OSVR_DEV_VERBOSE("Error creating skeleton object - null skeleton
        /// config returned");
        return OSVR_RETURN_FAILURE;
    }
    if (!skeleton->cfg) {
        /// OSVR_DEV_VERBOSE("Error creating skeleton config - null internal
        /// skeleton config object returned");
        return OSVR_RETURN_FAILURE;
    }
    ctx->acquireObject(skeleton);
    *skel = skeleton.get();
    return OSVR_RETURN_SUCCESS;
}

#define OSVR_VALIDATE_SKELETON_CONFIG                                          \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == skel) {                                                     \
        OSVR_DEV_VERBOSE("Passed a null skeleton object!");                    \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeSkeleton(OSVR_Skeleton skel) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_ClientContext ctx = skel->m_ctx;
    BOOST_ASSERT_MSG(
        ctx != nullptr,
        "Should never get a skeleton object with a null context in it.");
    if (nullptr == ctx) {
        return OSVR_RETURN_FAILURE;
    }
    auto freed = ctx->releaseObject(skel);
    return freed ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientGetSkeletonBoneId(OSVR_Skeleton skel,
                                            const char *boneName,
                                            OSVR_SkeletonBoneCount *boneId) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(boneId, "bone Id");
    if (!skel->cfg->getBoneId(boneName, boneId)) {
        OSVR_DEV_VERBOSE("Error getting boneId for " << boneName);
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonStringBoneNameLength(
    OSVR_Skeleton skel, OSVR_SkeletonBoneCount boneId, uint32_t *len) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(len, "name length");
    try {
        auto boneName = skel->cfg->getBoneName(boneId);
        *len = static_cast<uint32_t>(boneName.empty() ? 0 : (boneName.size() + 1));
    } catch (osvr::client::IdNotFound &) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided boneId : Id not found");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided boneId : " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonBoneName(OSVR_Skeleton skel,
                                              OSVR_SkeletonBoneCount boneId,
                                              char *boneName, uint32_t len) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(boneName, "bone name");
    try {
        auto name = skel->cfg->getBoneName(boneId);
        if (name.size() + 1 > len) {
            /// buffer too small
            return OSVR_RETURN_FAILURE;
        }
        // this line pops warning 4996 on VS compiler, disabled above
        name.copy(boneName, name.size());
        boneName[name.size()] = '\0';
    } catch (osvr::client::IdNotFound &) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided boneId : Id not found");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided boneId : " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonBoneState(OSVR_Skeleton skel,
                                               OSVR_SkeletonBoneCount boneId,
                                               OSVR_SkeletonBoneState *state) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(state, "bone state");
    try {
        OSVR_Pose3 pose = skel->cfg->getBoneState(boneId);
        state->boneId = boneId;
        state->pose = pose;
    } catch (osvr::client::NoPoseYet &) {
        OSVR_DEV_VERBOSE(
            "Error getting pose for bone id: no pose yet available");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE("Error getting bone pose - exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_SUCCESS;
}

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonAvailableJointId(
    OSVR_Skeleton skel, OSVR_SkeletonJointCount jointIndex,
    OSVR_SkeletonJointCount *jointId)
{
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(jointId, "joint Id");
    if (!skel->cfg->getAvailableJointId(jointIndex, jointId)) {
        OSVR_DEV_VERBOSE("ERROR getting the jointId for jointIndex " << jointIndex);
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetSkeletonAvailableBoneId(
    OSVR_Skeleton skel, OSVR_SkeletonBoneCount boneIndex,
    OSVR_SkeletonBoneCount *boneId)
{
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(boneId, "bone Id");
    if (!skel->cfg->getAvailableBoneId(boneIndex, boneId)) {
        OSVR_DEV_VERBOSE("ERROR getting the boneId for boneIndex " << boneIndex);
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonJointId(OSVR_Skeleton skel,
                                             const char *jointName,
                                             OSVR_SkeletonJointCount *jointId) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(jointId, "joint Id");
    if (!skel->cfg->getJointId(jointName, jointId)) {
        OSVR_DEV_VERBOSE("Error getting jointId for " << jointName);
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonStringJointNameLength(
    OSVR_Skeleton skel, OSVR_SkeletonJointCount jointId, uint32_t *len) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(len, "name length");
    try {
        auto jointName = skel->cfg->getJointName(jointId);
        *len = static_cast<uint32_t>(jointName.empty() ? 0 : (jointName.size() + 1));
    } catch (osvr::client::IdNotFound &) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided jointId : Id not found");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided jointId : " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetSkeletonJointName(OSVR_Skeleton skel,
                                               OSVR_SkeletonJointCount jointId,
                                               char *jointName, uint32_t len) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(jointName, "joint name");
    try {
        auto name = skel->cfg->getJointName(jointId);
        if (name.size() + 1 > len) {
            /// buffer too small
            return OSVR_RETURN_FAILURE;
        }
        // this line pops warning 4996 on VS compiler, disabled above
        name.copy(jointName, name.size());
        jointName[name.size()] = '\0';
    } catch (osvr::client::IdNotFound &) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided jointId : Id not found");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error getting name for provided jointId : " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrClientGetSkeletonJointState(OSVR_Skeleton skel,
                                OSVR_SkeletonJointCount jointId,
                                OSVR_SkeletonJointState *state) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(state, "joint state");
    try {
        OSVR_Pose3 pose = skel->cfg->getJointState(jointId);
        state->jointId = jointId;
        state->pose = pose;
    } catch (osvr::client::NoPoseYet &) {
        OSVR_DEV_VERBOSE("Error getting pose for joint: no pose yet available");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE("Error getting joint pose - exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrClientGetSkeletonNumBones(OSVR_Skeleton skel,
                              OSVR_SkeletonBoneCount *numBones) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(numBones, "number of bones");
    *numBones = skel->cfg->getNumBones();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrClientGetSkeletonNumJoints(OSVR_Skeleton skel,
                               OSVR_SkeletonJointCount *numJoints) {
    OSVR_VALIDATE_SKELETON_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(numJoints, "number of joints");
    *numJoints = skel->cfg->getNumJoints();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetChildJointId(
    OSVR_ClientInterface skeletonIface, OSVR_SkeletonJointCount jointId,
    OSVR_SkeletonJointCount childNum, OSVR_SkeletonJointCount *childJointId) {
    OSVR_VALIDATE_OUTPUT_PTR(childJointId, "child joint id");
    return OSVR_RETURN_SUCCESS;
}
