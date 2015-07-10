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

#ifndef INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF
#define INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF

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

    struct LocationData {
        OSVR_ChannelCount sensor;
        OSVR_Location2DState location;
    };

    namespace messages {
        class LocationRecord : public MessageRegistration<LocationRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class Location2DComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<Location2DComponent>
        create(OSVR_ChannelCount numSensor = 1);

        /// @brief Message from server to client, containing 2D location data.
        messages::LocationRecord locationRecord;

        OSVR_COMMON_EXPORT void
        sendLocationData(OSVR_Location2DState location,
                         OSVR_ChannelCount sensor,
                         OSVR_TimeValue const &timestamp);

        typedef std::function<void(LocationData const &,
                                   util::time::TimeValue const &)>
            LocationHandler;
        OSVR_COMMON_EXPORT void registerLocationHandler(LocationHandler cb);

      private:
        Location2DComponent(OSVR_ChannelCount numChan);
        virtual void m_parentSet();

        static int VRPN_CALLBACK
        m_handleLocationRecord(void *userdata, vrpn_HANDLERPARAM p);

        void m_checkFirst(OSVR_Location2DState const &location);

        OSVR_ChannelCount m_numSensor;
        std::vector<LocationHandler> m_cb;
        bool m_gotOne;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF
