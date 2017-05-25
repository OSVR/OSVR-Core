/** @file
    @brief Implementation

    @date 2017

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2017 Sensics, Inc.
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
#include <osvr/Client/Skeleton.h>
#include <osvr/Util/Verbosity.h>
// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_SkeletonObject::OSVR_SkeletonObject(
    OSVR_ClientContext ctx, osvr::common::PathTree const &articulationTree)
    : m_cfg(
          osvr::client::SkeletonConfigFactory::create(ctx, articulationTree)) {
    OSVR_DEV_VERBOSE("Created OSVR_SkeletonObject!");
}

OSVR_SkeletonObject::~OSVR_SkeletonObject() {
    OSVR_DEV_VERBOSE("Destroyed OSVR_SkeletonObject!");
}

inline bool
OSVR_SkeletonObject::getBoneId(const char *boneName,
                               OSVR_SkeletonBoneCount *boneId) const {
    return m_cfg->getBoneId(boneName, boneId);
}

inline bool
OSVR_SkeletonObject::getAvailableBoneId(OSVR_SkeletonBoneCount boneIndex,
                                        OSVR_SkeletonBoneCount *boneId) const {
    return m_cfg->getAvailableBoneId(boneIndex, boneId);
}
inline bool OSVR_SkeletonObject::getAvailableJointId(
    OSVR_SkeletonJointCount jointIndex,
    OSVR_SkeletonJointCount *jointId) const {
    return m_cfg->getAvailableJointId(jointIndex, jointId);
}
inline bool
OSVR_SkeletonObject::getJointId(const char *jointName,
                                OSVR_SkeletonJointCount *jointId) const {
    return m_cfg->getJointId(jointName, jointId);
}
inline OSVR_Pose3
OSVR_SkeletonObject::getJointState(OSVR_SkeletonJointCount jointId) const {
    return m_cfg->getJointState(jointId);
}
inline OSVR_Pose3
OSVR_SkeletonObject::getBoneState(OSVR_SkeletonBoneCount boneId) const {
    return m_cfg->getBoneState(boneId);
}
inline OSVR_SkeletonBoneCount OSVR_SkeletonObject::getNumBones() const {
    return m_cfg->getNumBones();
}
inline OSVR_SkeletonJointCount OSVR_SkeletonObject::getNumJoints() const {
    return m_cfg->getNumJoints();
}
inline std::string
OSVR_SkeletonObject::getBoneName(OSVR_SkeletonBoneCount boneId) const {
    return m_cfg->getBoneName(boneId);
}
inline std::string
OSVR_SkeletonObject::getJointName(OSVR_SkeletonBoneCount boneId) const {
    return m_cfg->getJointName(boneId);
}
inline void OSVR_SkeletonObject::updateArticulationSpec(
    osvr::common::PathTree const &articulationTree) {
    m_cfg->updateArticulationTree(articulationTree);
}
inline void OSVR_SkeletonObject::updateSkeletonPoses() {
    m_cfg->updateSkeletonPoses();
}