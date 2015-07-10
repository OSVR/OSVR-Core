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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_EyeTrackerComponent_h_GUID_68C4C9C6_84AF_43AC_1392_008C04EFAF0D
#define INCLUDED_EyeTrackerComponent_h_GUID_68C4C9C6_84AF_43AC_1392_008C04EFAF0D

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

    struct OSVR_EyeNotification {
        OSVR_ChannelCount sensor;
    };

    namespace messages {
        class EyeRegion : public MessageRegistration<EyeRegion> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class EyeTrackerComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<EyeTrackerComponent>
        create(OSVR_ChannelCount numSensor = 2);

        /// @brief Message from server to client, containing eye data.
        messages::EyeRegion eyeRegion;

        OSVR_COMMON_EXPORT void
        sendNotification(OSVR_ChannelCount sensor,
                         OSVR_TimeValue const &timestamp);

        typedef std::function<void(OSVR_EyeNotification const &,
                                   util::time::TimeValue const &)> EyeHandler;
        OSVR_COMMON_EXPORT void registerEyeHandler(EyeHandler cb);

      private:
        EyeTrackerComponent(OSVR_ChannelCount numChan);
        virtual void m_parentSet();

        static int VRPN_CALLBACK
        m_handleEyeRegion(void *userdata, vrpn_HANDLERPARAM p);

        OSVR_ChannelCount m_numSensor;
        std::vector<EyeHandler> m_cb;
        bool m_gotOne;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_EyeTrackerComponent_h_GUID_68C4C9C6_84AF_43AC_1392_008C04EFAF0D
