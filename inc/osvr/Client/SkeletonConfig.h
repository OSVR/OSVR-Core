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
#include <osvr/Client/DisplayInput.h>
#include <osvr/Client/Viewer.h>
#include <osvr/Client/Viewers.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/RegisteredStringMap.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Util/ContainerWrapper.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace client {

    typedef std::vector<std::pair<util::StringID, common::ClientInterfacePtr>>
        InterfaceMap;

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
        create(OSVR_ClientContext ctx, OSVR_ClientInterface iface);
    };

    class SkeletonConfig {
      public:
        // non-copyable
        SkeletonConfig(SkeletonConfig const &) = delete;
        SkeletonConfig &operator=(SkeletonConfig const &) = delete;

        bool getBoneId(const char *boneName, OSVR_SkeletonBoneCount *boneId);
        bool getJointId(const char *jointName,
                        OSVR_SkeletonJointCount *jointId);

        std::string const getBoneName(OSVR_SkeletonBoneCount boneId);
        std::string const getJointName(OSVR_SkeletonJointCount jointId);

        OSVR_Pose3 getBoneState(OSVR_SkeletonBoneCount boneId);
        OSVR_Pose3 getJointState(OSVR_SkeletonJointCount jointId);

        OSVR_SkeletonBoneCount getNumBones();
        OSVR_SkeletonJointCount getNumJoints();

      private:
        friend class SkeletonConfigFactory;
        SkeletonConfig(OSVR_ClientContext ctx, OSVR_ClientInterface iface);
        /// @brief check if the articulation tree has been updated, then we need
        /// to traverse the articulation tree again, and update values
        bool isSkeletonTreeUpdated() const;
        OSVR_ClientContext m_ctx;
        OSVR_ClientInterface m_iface;
        osvr::common::PathTree m_articulationTree;
        osvr::common::RegisteredStringMap m_jointMap;
        osvr::common::RegisteredStringMap m_boneMap;
        InterfaceMap m_jointInterfaces;
        InterfaceMap m_boneInterfaces;
    };

    inline bool SkeletonConfig::getBoneId(const char *boneName,
                                          OSVR_SkeletonBoneCount *boneId) {

        osvr::util::StringID id = m_boneMap.getStringID(boneName);
        if (id.empty()) {
            return false;
        } else {
            *boneId = id.value();
            return true;
        }
    }

    inline bool SkeletonConfig::getJointId(const char *jointName,
                                           OSVR_SkeletonBoneCount *jointId) {

        osvr::util::StringID id = m_jointMap.getStringID(jointName);
        if (id.empty()) {
            return false;
        } else {
            *jointId = id.value();
            return true;
        }
    }

    inline OSVR_Pose3
    SkeletonConfig::getJointState(OSVR_SkeletonJointCount jointId) {

        OSVR_TimeValue timestamp;
        OSVR_Pose3 pose = { 0 };
        osvrPose3SetIdentity(&pose);

        // find an interface for given jointId
        for (auto val : m_jointInterfaces) {
            if (val.first.value() == jointId) {
                // try to get state, if available
                bool hasState =
                    val.second->getState<OSVR_PoseReport>(timestamp, pose);
                if (!hasState) {
                    throw NoPoseYet();
                }
                return pose;
            }
        }

        throw NoPoseYet();
    }

    inline OSVR_Pose3
    SkeletonConfig::getBoneState(OSVR_SkeletonBoneCount boneId) {

        OSVR_TimeValue timestamp;
        OSVR_Pose3 pose = { 0 };
        osvrPose3SetIdentity(&pose);

        // find an interface for given boneId
        for (auto val : m_boneInterfaces) {
            if (val.first.value() == boneId) {
                // try to get state, if available
                bool hasState =
                    val.second->getState<OSVR_PoseReport>(timestamp, pose);
                if (!hasState) {
                    throw NoPoseYet();
                }
                return pose;
            }
        }

        throw NoPoseYet();
    }

    inline OSVR_SkeletonBoneCount SkeletonConfig::getNumBones() {
        return static_cast<OSVR_SkeletonBoneCount>(m_boneMap.getEntries().size());
    }

    inline OSVR_SkeletonBoneCount SkeletonConfig::getNumJoints() {
        return static_cast<OSVR_SkeletonBoneCount>(m_jointMap.getEntries().size());
    }

    inline std::string const
    SkeletonConfig::getBoneName(OSVR_SkeletonBoneCount boneId) {
        auto boneName = m_boneMap.getStringFromId(util::StringID(boneId));
        if (boneName.empty()) {
            throw IdNotFound();
        }
        return boneName;
    }
    inline std::string const
    SkeletonConfig::getJointName(OSVR_SkeletonJointCount jointId) {
        auto jointName = m_jointMap.getStringFromId(util::StringID(jointId));
        if (jointName.empty()) {
            throw IdNotFound();
        }
        return jointName;
    }

} // namespace client
} // namespace osvr

#endif // INCLUDED_SkeletonConfig_h_GUID_A1D6BCE7_D044_4582_9BA5_860C790A3013
