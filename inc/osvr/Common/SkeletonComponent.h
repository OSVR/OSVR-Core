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

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    struct SkeletonNotification {
        OSVR_ChannelCount sensor;
    };

    namespace messages {
        class SkeletonRecord : public MessageRegistration<SkeletonRecord> {
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
        create(OSVR_ChannelCount numSensor = 1);

        /// @brief Message from server to client, containing skeleton
        /// notification.
        messages::SkeletonRecord skeletonRecord;

        OSVR_COMMON_EXPORT void
        sendNotification(OSVR_ChannelCount sensor,
                         OSVR_TimeValue const &timestamp);

        typedef std::function<void(SkeletonNotification const &,
                                   util::time::TimeValue const &)>
            SkeletonHandler;
        OSVR_COMMON_EXPORT void registerSkeletonHandler(SkeletonHandler cb);

      private:
        SkeletonComponent(OSVR_ChannelCount numChan);
        virtual void m_parentSet();

        static int VRPN_CALLBACK m_handleSkeletonRecord(void *userdata,
                                                        vrpn_HANDLERPARAM p);

        OSVR_ChannelCount m_numSensor;
        std::vector<SkeletonHandler> m_cb;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_SkeletonComponent_h_GUID_53FA481D_05BD_4CA5_3A50_FE01B600476E
