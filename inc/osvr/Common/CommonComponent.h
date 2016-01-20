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

#ifndef INCLUDED_CommonComponent_h_GUID_199AFD54_3489_4295_30F9_00B7CE33FE31
#define INCLUDED_CommonComponent_h_GUID_199AFD54_3489_4295_30F9_00B7CE33FE31

// Internal Includes
#include <osvr/Common/CommonComponent_fwd.h>
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <functional>

namespace osvr {
namespace common {
    namespace messages {
        class VRPNPing : public MessageRegistration<VRPNPing> {
          public:
            static const char *identifier();
        };
        class VRPNPong : public MessageRegistration<VRPNPong> {
          public:
            static const char *identifier();
        };
        class VRPNGotFirstConnection
            : public MessageRegistration<VRPNGotFirstConnection> {
          public:
            static const char *identifier();
        };
        class VRPNGotConnection
            : public MessageRegistration<VRPNGotConnection> {
          public:
            static const char *identifier();
        };
        class VRPNDroppedConnection
            : public MessageRegistration<VRPNDroppedConnection> {
          public:
            static const char *identifier();
        };
        class VRPNDroppedLastConnection
            : public MessageRegistration<VRPNDroppedLastConnection> {
          public:
            static const char *identifier();
        };
    } // namespace messages
    /// @brief BaseDevice component, for the VRPN built-in common messages
    class CommonComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<CommonComponent> create();

        typedef std::function<void()> Handler;

        messages::VRPNPing ping;

        /// @brief Register a ping handler: a ping is sent from a client device
        /// to the corresponding server device upon connection or reconnection.
        OSVR_COMMON_EXPORT void registerPingHandler(Handler const &handler);

        messages::VRPNPong pong;

        /// @brief Register a pong handler: a pong replies to a ping, and  is
        /// sent from a server device to the corresponding client device.
        OSVR_COMMON_EXPORT void registerPongHandler(Handler const &handler);

      private:
        CommonComponent();
        virtual void m_parentSet();
        static int VRPN_CALLBACK
        m_handlePing(void *userdata, vrpn_HANDLERPARAM);
        static int VRPN_CALLBACK
        m_handlePong(void *userdata, vrpn_HANDLERPARAM);

        std::vector<Handler> m_pingHandlers;
        std::vector<Handler> m_pongHandlers;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_CommonComponent_h_GUID_199AFD54_3489_4295_30F9_00B7CE33FE31
