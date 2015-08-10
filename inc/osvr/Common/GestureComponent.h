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

#ifndef INCLUDED_GestureComponent_h_GUID_F9F3BC3E_1DB2_498F_FD8E_273A269220DA
#define INCLUDED_GestureComponent_h_GUID_F9F3BC3E_1DB2_498F_FD8E_273A269220DA

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Common/RegisteredStringMap.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Common/JSONSerializationTags.h>

#include <osvr/Common/SystemComponent.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    struct GestureData {
        OSVR_ChannelCount sensor;
        OSVR_GestureState gestureState;
        StringID gestureID;
    };

    namespace messages {
        class GestureRecord : public MessageRegistration<GestureRecord> {
          public:
            class MessageSerialization;

            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class GestureComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<GestureComponent>
            create(SystemComponentPtr sysComp);

        //OSVR_COMMON_EXPORT ~GestureComponent();

        /// @brief Message from server to client, containing Gesture data.
        messages::GestureRecord gestureRecord;

		OSVR_COMMON_EXPORT void sendGestureData(
			OSVR_GestureState gestureState, OSVR_GestureID gestureID,
			OSVR_ChannelCount sensor, OSVR_TimeValue const &timestamp);

        void m_sendGestureMap(OSVR_TimeValue const &timestamp);

        typedef std::function<void(
            GestureData const &, util::time::TimeValue const &)> GestureHandler;

        OSVR_COMMON_EXPORT void registerGestureHandler(GestureHandler cb);

		OSVR_COMMON_EXPORT OSVR_GestureID getGestureID(const char *gestureName);

      private:
        GestureComponent(SystemComponentPtr sysComp);

        virtual void m_parentSet();

        static int VRPN_CALLBACK
        m_handleGestureRecord(void *userdata, vrpn_HANDLERPARAM p);

        void m_checkFirst(OSVR_GestureState const &gesture);

        OSVR_ChannelCount m_numSensor;
        std::vector<GestureHandler> m_cb;

        // name to ID map used by the server
        //RegisteredStringMap m_gestureNameMap;
		MapPtr m_gestureNameMap;

		/// @brief System device component
		shared_ptr<SystemComponent> m_sysComponent;
		
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_GestureComponent_h_GUID_F9F3BC3E_1DB2_498F_FD8E_273A269220DA
