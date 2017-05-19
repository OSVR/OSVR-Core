/** @file
    @brief Header

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

#ifndef INCLUDED_Skeleton_h_GUID_290D260E_2590_4B92_802E_9F90CA860F8B
#define INCLUDED_Skeleton_h_GUID_290D260E_2590_4B92_802E_9F90CA860F8B

// Internal Includes
#include <osvr/Client/SkeletonConfig.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_SkeletonObject {
  public:
    OSVR_CLIENT_EXPORT
    OSVR_SkeletonObject(OSVR_ClientContext ctx,
                        osvr::common::PathTree const &articulationTree);
    OSVR_CLIENT_EXPORT ~OSVR_SkeletonObject();

    OSVR_CLIENT_EXPORT bool getBoneId(const char *boneName,
                                      OSVR_SkeletonBoneCount *boneId);
    OSVR_CLIENT_EXPORT bool getAvailableBoneId(OSVR_SkeletonBoneCount boneIndex,
                                               OSVR_SkeletonBoneCount *boneId);
    OSVR_CLIENT_EXPORT bool
    getAvailableJointId(OSVR_SkeletonJointCount jointIndex,
                        OSVR_SkeletonJointCount *jointId);
    OSVR_CLIENT_EXPORT bool getJointId(const char *jointName,
                                       OSVR_SkeletonJointCount *jointId);
    OSVR_CLIENT_EXPORT OSVR_Pose3
    getJointState(OSVR_SkeletonJointCount jointId);
    OSVR_CLIENT_EXPORT OSVR_Pose3 getBoneState(OSVR_SkeletonBoneCount boneId);
    OSVR_CLIENT_EXPORT OSVR_SkeletonBoneCount getNumBones();
    OSVR_CLIENT_EXPORT OSVR_SkeletonJointCount getNumJoints();
    OSVR_CLIENT_EXPORT std::string const
    getBoneName(OSVR_SkeletonBoneCount boneId);
    OSVR_CLIENT_EXPORT std::string const
    getJointName(OSVR_SkeletonBoneCount boneId);
    OSVR_CLIENT_EXPORT void
    updateArticulationSpec(osvr::common::PathTree const &articulationTree);
    OSVR_CLIENT_EXPORT void updateSkeletonPoses();

  private:
    OSVR_ClientContext m_ctx = nullptr;
    osvr::client::SkeletonConfigPtr m_cfg = nullptr;
};

#endif // INCLUDED_Skeleton_h_GUID_290D260E_2590_4B92_802E_9F90CA860F8B
