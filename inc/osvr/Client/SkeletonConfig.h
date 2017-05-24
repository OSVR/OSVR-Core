/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_SkeletonConfig_h_GUID_A1D6BCE7_D044_4582_9BA5_860C790A3013
#define INCLUDED_SkeletonConfig_h_GUID_A1D6BCE7_D044_4582_9BA5_860C790A3013

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Client/ViewerEye.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/RegisteredStringMap.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ContainerWrapper.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace osvr {
namespace client {

    typedef std::vector<std::pair<util::StringID, InternalInterfaceOwner>>
        InterfaceMap;
    typedef std::vector<std::pair<util::StringID, OSVR_Pose3>> PoseMap;

    struct NoCtxYet : std::runtime_error {
        NoCtxYet()
            : std::runtime_error("Client context is not yet initialized!") {}
    };

    struct IdNotFound : std::runtime_error {
        IdNotFound()
            : std::runtime_error(
                  "Could not find joint/bone Id with provided info!") {}
    };

    class SkeletonConfig;
    typedef unique_ptr<SkeletonConfig> SkeletonConfigPtr;
    class SkeletonConfigFactory {
      public:
        OSVR_CLIENT_EXPORT static SkeletonConfigPtr
        create(OSVR_ClientContext ctx,
               osvr::common::PathTree const &articulationTree);
    };

    class SkeletonConfig {
      public:
        // non-copyable
        SkeletonConfig(SkeletonConfig const &) = delete;
        SkeletonConfig &operator=(SkeletonConfig const &) = delete;

        bool getBoneId(const char *boneName,
                       OSVR_SkeletonBoneCount *boneId) const;
        bool getJointId(const char *jointName,
                        OSVR_SkeletonJointCount *jointId) const;

        bool getAvailableJointId(OSVR_SkeletonJointCount jointIndex,
                                 OSVR_SkeletonJointCount *jointId) const;

        bool getAvailableBoneId(OSVR_SkeletonBoneCount boneIndex,
                                OSVR_SkeletonBoneCount *boneId) const;

        std::string getBoneName(OSVR_SkeletonBoneCount boneId) const;
        std::string getJointName(OSVR_SkeletonJointCount jointId) const;

        OSVR_Pose3 getBoneState(OSVR_SkeletonBoneCount boneId) const;
        OSVR_Pose3 getJointState(OSVR_SkeletonJointCount jointId) const;

        OSVR_SkeletonBoneCount getNumBones() const;
        OSVR_SkeletonJointCount getNumJoints() const;
        void
        updateArticulationTree(osvr::common::PathTree const &articulationTree);
        /* @brief Go thru the joint and bone interfaces and set the poses
         */
        void updateSkeletonPoses();

      private:
        friend class SkeletonConfigFactory;
        SkeletonConfig(OSVR_ClientContext ctx);
        /// @brief check if the articulation tree has been updated, then we need
        /// to traverse the articulation tree again, and update values
        bool isSkeletonTreeUpdated() const;
        OSVR_ClientContext m_ctx;
        osvr::common::PathTree m_articulationTree;
        osvr::common::RegisteredStringMap m_jointMap;
        osvr::common::RegisteredStringMap m_boneMap;
        InterfaceMap m_jointInterfaces;
        InterfaceMap m_boneInterfaces;
        PoseMap m_jointPoses;
        PoseMap m_bonePoses;
    };

    inline bool
    SkeletonConfig::getBoneId(const char *boneName,
                              OSVR_SkeletonBoneCount *boneId) const {
        if (boneId == nullptr) {
            return false;
        }
        osvr::util::StringID id = m_boneMap.getStringID(boneName);
        if (id.empty()) {
            return false;
        }
        *boneId = id.value();
        return true;
    }

    inline bool SkeletonConfig::getAvailableJointId(
        OSVR_SkeletonJointCount jointIndex,
        OSVR_SkeletonJointCount *jointId) const {
        if (jointId == nullptr) {
            return false;
        }
        *jointId = 0;
        if (jointIndex >= m_jointInterfaces.size()) {
            return false;
        }

        // m_jointInterfaces[jointIndex].first.value() is the return value,
        // but we need to make sure we actually allocate an entry for it

        // get the string id
        auto stringId =
            m_jointMap.getStringFromId(m_jointInterfaces[jointIndex].first);

        // then get the joint id
        auto ret = getJointId(stringId.c_str(), jointId);

        return ret;
    }

    inline bool
    SkeletonConfig::getAvailableBoneId(OSVR_SkeletonBoneCount boneIndex,
                                       OSVR_SkeletonBoneCount *boneId) const {
        if (boneId == nullptr) {
            return false;
        }
        *boneId = 0;
        if (boneIndex >= m_boneMap.getEntries().size()) {
            return false;
        }

        auto boneName = m_boneMap.getEntries().at(boneIndex);

        auto ret = getBoneId(boneName.c_str(), boneId);

        return ret;
    }

    inline bool
    SkeletonConfig::getJointId(const char *jointName,
                               OSVR_SkeletonJointCount *jointId) const {
        if (jointId == nullptr) {
            return false;
        }
        osvr::util::StringID id = m_jointMap.getStringID(jointName);
        if (id.empty()) {
            return false;
        }
        *jointId = id.value();
        return true;
    }

    inline OSVR_Pose3
    SkeletonConfig::getJointState(OSVR_SkeletonJointCount jointId) const {

        // find an interface for given jointId
        for (auto val : m_jointPoses) {
            if (val.first.value() == jointId) {
                // return the joint state
                return val.second;
            }
        }
        // pose not available for this frame
        throw NoPoseYet();
    }

    inline OSVR_Pose3
    SkeletonConfig::getBoneState(OSVR_SkeletonBoneCount boneId) const {
        /// @todo should be returning derived pose

        // find an interface for given boneId
        for (auto val : m_bonePoses) {
            if (val.first.value() == boneId) {
                // return the bone state
                return val.second;
            }
        }
        // pose not available for this frame
        throw NoPoseYet();
    }

    inline OSVR_SkeletonBoneCount SkeletonConfig::getNumBones() const {
        return static_cast<OSVR_SkeletonBoneCount>(
            m_boneMap.getEntries().size());
    }

    inline OSVR_SkeletonJointCount SkeletonConfig::getNumJoints() const {
        return static_cast<OSVR_SkeletonJointCount>(
            m_jointMap.getEntries().size());
    }

    inline std::string
    SkeletonConfig::getBoneName(OSVR_SkeletonBoneCount boneId) const {
        auto boneName = m_boneMap.getStringFromId(util::StringID(boneId));
        if (boneName.empty()) {
            throw IdNotFound();
        }
        return boneName;
    }
    inline std::string
    SkeletonConfig::getJointName(OSVR_SkeletonJointCount jointId) const {
        auto jointName = m_jointMap.getStringFromId(util::StringID(jointId));
        if (jointName.empty()) {
            throw IdNotFound();
        }
        return jointName;
    }
    inline void SkeletonConfig::updateSkeletonPoses() {

        // clear old values
        m_jointPoses.clear();
        m_bonePoses.clear();

        for (auto &val : m_jointInterfaces) {
            OSVR_TimeValue timestamp;
            OSVR_Pose3 pose;
            osvrPose3SetIdentity(&pose);
            if (val.second->getState<OSVR_PoseReport>(timestamp, pose)) {
                m_jointPoses.push_back(std::make_pair(val.first, pose));
            }
        }
        for (auto &val : m_boneInterfaces) {
            OSVR_TimeValue timestamp;
            OSVR_Pose3 pose;
            osvrPose3SetIdentity(&pose);
            if (val.second->getState<OSVR_PoseReport>(timestamp, pose)) {
                m_bonePoses.push_back(std::make_pair(val.first, pose));
            }
        }
    }

} // namespace client
} // namespace osvr

#endif // INCLUDED_SkeletonConfig_h_GUID_A1D6BCE7_D044_4582_9BA5_860C790A3013
