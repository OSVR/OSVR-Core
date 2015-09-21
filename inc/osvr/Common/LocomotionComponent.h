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

#ifndef INCLUDED_LocomotionComponent_h_GUID_60F8A9AB_1531_4AB4_E842_89EBB4851EE5
#define INCLUDED_LocomotionComponent_h_GUID_60F8A9AB_1531_4AB4_E842_89EBB4851EE5

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

#include <osvr/Common/CommonComponent_fwd.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    struct NaviVelocityData {
        OSVR_ChannelCount sensor;
        OSVR_NaviVelocityState naviVelState;
    };

    struct NaviPositionData {
        OSVR_ChannelCount sensor;
        OSVR_NaviPositionState naviPosnState;
    };

    namespace messages {
        class NaviVelocityRecord
            : public MessageRegistration<NaviVelocityRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

        class NaviPositionRecord
            : public MessageRegistration<NaviPositionRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class LocomotionComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<LocomotionComponent> create();

        /// @brief Message from server to client, containing navigation velocity
        messages::NaviVelocityRecord naviVelRecord;

        /// @brief Optional message from server to client, containing navigation
        /// position
        messages::NaviPositionRecord naviPosnRecord;

        OSVR_COMMON_EXPORT void
        sendNaviVelocityData(OSVR_NaviVelocityState naviVelState,
                             OSVR_ChannelCount sensor,
                             OSVR_TimeValue const &timestamp);

        OSVR_COMMON_EXPORT void
        sendNaviPositionData(OSVR_NaviPositionState naviPosnState,
                             OSVR_ChannelCount sensor,
                             OSVR_TimeValue const &timestamp);

        typedef std::function<void(NaviVelocityData const &,
                                   util::time::TimeValue const &)>
            NaviVelocityHandler;

        typedef std::function<void(NaviPositionData const &,
                                   util::time::TimeValue const &)>
            NaviPositionHandler;

        OSVR_COMMON_EXPORT void
        registerNaviVelocityHandler(NaviVelocityHandler cb);
        OSVR_COMMON_EXPORT void
        registerNaviPositionHandler(NaviPositionHandler cb);

      private:
        LocomotionComponent();
        virtual void m_parentSet();

        static int VRPN_CALLBACK
        m_handleNaviVelocityRecord(void *userdata, vrpn_HANDLERPARAM p);
        static int VRPN_CALLBACK
        m_handleNaviPositionRecord(void *userdata, vrpn_HANDLERPARAM p);

        std::vector<NaviVelocityHandler> m_cb_vel;
        std::vector<NaviPositionHandler> m_cb_posn;

        /// @brief Common component for system device
        common::CommonComponent *m_commonComponent;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_LocomotionComponent_h_GUID_60F8A9AB_1531_4AB4_E842_89EBB4851EE5
