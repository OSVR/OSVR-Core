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

#ifndef INCLUDED_SkeletonComponent_h_GUID_53FA481D_05BD_4CA5_3A50_FE01B600476E
#define INCLUDED_SkeletonComponent_h_GUID_53FA481D_05BD_4CA5_3A50_FE01B600476E

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Common/PathTree.h>

#include <osvr/Common/CommonComponent_fwd.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    struct SkeletonNotification {
        OSVR_ChannelCount sensor;
    };

    struct SkeletonSpec {
        Json::Value spec;
    };

    namespace messages {
        class SkeletonRecord : public MessageRegistration<SkeletonRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };
        class SkeletonSpecRecord
            : public MessageRegistration<SkeletonSpecRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class SkeletonComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<SkeletonComponent>
        create(std::string const &jsonSpec, OSVR_ChannelCount numSensor = 1);

        /// @brief Message from server to client, containing skeleton
        /// notification.
        messages::SkeletonRecord skeletonRecord;

        /// @brief Message from server to client, containing new/updated
        /// skeleton articulation spec
        messages::SkeletonSpecRecord skeletonSpecRecord;

        /// @brief Sends a notification to the client that tracker reports for
        /// given skeleton had finished reporting and it can coalesce reports
        /// now
        OSVR_COMMON_EXPORT void
        sendNotification(OSVR_ChannelCount sensor,
                         OSVR_TimeValue const &timestamp);

        /// @brief Sends a new or updated articulation specification to the
        /// client. Used by plugins and internally when client first connects
        OSVR_COMMON_EXPORT void sendArticulationSpec(std::string const &spec);

        /// @brief Sets the articultion specification. Should be used during the creation of skeleton component.
        OSVR_COMMON_EXPORT OSVR_ReturnCode setArticulationSpec(std::string const &jsonDescriptor);

        /// @brief Sets the articultion specification. This will auto-update the articulation tree
        OSVR_COMMON_EXPORT OSVR_ReturnCode setArticulationSpec(std::string const &jsonDescriptor, std::string const &deviceName);

        typedef std::function<void(SkeletonNotification const &,
                                   util::time::TimeValue const &)>
            SkeletonHandler;
        typedef std::function<void(SkeletonSpec const &,
                                   util::time::TimeValue const &)>
            SkeletonSpecHandler;
        OSVR_COMMON_EXPORT void registerSkeletonHandler(SkeletonHandler cb);
        OSVR_COMMON_EXPORT void
        registerSkeletonSpecHandler(SkeletonSpecHandler cb);

        OSVR_COMMON_EXPORT PathTree const &getArticulationTree() const;
        OSVR_COMMON_EXPORT PathTree &getArticulationTree();

      private:
        SkeletonComponent(std::string const &jsonSpec, OSVR_ChannelCount numChan);
        virtual void m_parentSet();

        static int VRPN_CALLBACK m_handleSkeletonRecord(void *userdata,
                                                        vrpn_HANDLERPARAM p);
        static int VRPN_CALLBACK
        m_handleSkeletonSpecRecord(void *userdata, vrpn_HANDLERPARAM p);
        OSVR_ChannelCount m_numSensor;
        /// @brief Articulation specs
        std::string m_spec;
        std::vector<SkeletonHandler> m_cb;
        std::vector<SkeletonSpecHandler> m_cb_spec;

        /// @brief Common component for system device
        common::CommonComponent *m_commonComponent;

        common::PathTree m_articulationTree;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_SkeletonComponent_h_GUID_53FA481D_05BD_4CA5_3A50_FE01B600476E
