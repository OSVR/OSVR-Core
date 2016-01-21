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
#include <osvr/TypePack/List.h>
#include <osvr/TypePack/TypeKeyedMap.h>

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
            OSVR_COMMON_EXPORT static const char *identifier();
        };
        class VRPNPong : public MessageRegistration<VRPNPong> {
          public:
            OSVR_COMMON_EXPORT static const char *identifier();
        };
        class VRPNGotFirstConnection
            : public MessageRegistration<VRPNGotFirstConnection> {
          public:
            OSVR_COMMON_EXPORT static const char *identifier();
        };
        class VRPNGotConnection
            : public MessageRegistration<VRPNGotConnection> {
          public:
            OSVR_COMMON_EXPORT static const char *identifier();
        };
        class VRPNDroppedConnection
            : public MessageRegistration<VRPNDroppedConnection> {
          public:
            OSVR_COMMON_EXPORT static const char *identifier();
        };
        class VRPNDroppedLastConnection
            : public MessageRegistration<VRPNDroppedLastConnection> {
          public:
            OSVR_COMMON_EXPORT static const char *identifier();
        };

        /// List of message types used in the CommonComponent that share the
        /// same handler behavior, so we can share registration behavior.
        using CommonComponentMessageTypes =
            typepack::list<VRPNPing, VRPNPong, VRPNGotFirstConnection,
                           VRPNGotConnection, VRPNDroppedConnection,
                           VRPNDroppedLastConnection>;
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

        /// @brief Register a pong handler: a pong replies to a ping, and is
        /// sent from a server device to the corresponding client device.
        OSVR_COMMON_EXPORT void registerPongHandler(Handler const &handler);

        messages::VRPNGotFirstConnection gotFirstConnection;
        messages::VRPNGotConnection gotConnection;
        messages::VRPNDroppedConnection droppedConnection;
        messages::VRPNDroppedLastConnection droppedLastConnection;

        template <typename T>
        void registerHandler(MessageRegistration<T> const &message,
                             Handler const &handler) {
            auto &handlers = typepack::get<T>(m_handlers);
            m_registerHandlerImpl(handlers, message.getMessageType(), handler);
        }

      private:
        CommonComponent();
        void m_parentSet() override;

        using HandlerList = std::vector<Handler>;
        OSVR_COMMON_EXPORT void
        m_registerHandlerImpl(HandlerList &handlers,
                              osvr::common::RawMessageType rawMessageType,
                              Handler const &handler);

        static int VRPN_CALLBACK m_baseHandler(void *userdata,
                                               vrpn_HANDLERPARAM);

        typepack::TypeKeyedMap<messages::CommonComponentMessageTypes,
                               std::vector<Handler>> m_handlers;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_CommonComponent_h_GUID_199AFD54_3489_4295_30F9_00B7CE33FE31
