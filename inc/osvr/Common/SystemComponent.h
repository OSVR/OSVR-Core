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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2
#define INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2

// Internal Includes
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Common/PathTree_fwd.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class RoutesFromServer : public MessageRegistration<RoutesFromServer> {
          public:
            class MessageSerialization;
            typedef void (*MessageHandler)(void *userdata,
                                           std::string const &message);

            static const char *identifier();
        };

        class AppStartupToServer
            : public MessageRegistration<AppStartupToServer> {
          public:
            static const char *identifier();
        };

        class ClientRouteToServer
            : public MessageRegistration<ClientRouteToServer> {
          public:
            class MessageSerialization;
            static const char *identifier();
        };

        class ReplacementTreeFromServer
            : public MessageRegistration<ReplacementTreeFromServer> {
          public:
            class MessageSerialization;
            static const char *identifier();
        };
    } // namespace messages

    /// @brief BaseDevice component, to be used only with the "OSVR" special
    /// device.
    class SystemComponent : public DeviceComponent {
      public:
        /// @brief Get the special device name to be used with this component.
        OSVR_COMMON_EXPORT static const char *deviceName();
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<SystemComponent> create();

        /// @brief Message from server to client, replacing all routes.
        messages::RoutesFromServer routesOut;

        OSVR_COMMON_EXPORT void sendRoutes(std::string const &routes);
        OSVR_COMMON_EXPORT void
        registerRoutesHandler(vrpn_MESSAGEHANDLER handler, void *userdata);

        /// @brief Message from client to server, notifying of app ID.
        messages::AppStartupToServer appStartup;

        /// @brief Message from client, sending a single new/updated JSON
        /// routing directive in to the server.
        messages::ClientRouteToServer routeIn;

        OSVR_COMMON_EXPORT void sendClientRouteUpdate(std::string const &route);
        OSVR_COMMON_EXPORT void
        registerClientRouteUpdateHandler(vrpn_MESSAGEHANDLER handler,
                                         void *userdata);

        /// @brief Message from server, updating/replacing the client's
        /// configuration
        messages::ReplacementTreeFromServer treeOut;

        typedef std::function<void(Json::Value const &,
                                   util::time::TimeValue const &)> JsonHandler;
        OSVR_COMMON_EXPORT void registerReplaceTreeHandler(JsonHandler cb);

        OSVR_COMMON_EXPORT void sendReplacementTree(PathTree &tree);

      private:
        SystemComponent();
        virtual void m_parentSet();
        static int VRPN_CALLBACK
        m_handleReplaceTree(void *userdata, vrpn_HANDLERPARAM p);

        std::vector<JsonHandler> m_replaceTreeHandlers;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2
