/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2
#define INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTraits.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class RoutesFromServer {
          public:
            class MessageSerialization {
              public:
                MessageSerialization(std::string const &str = std::string())
                    : m_str(str) {}

                template <typename T> void processMessage(T &p) {
                    p(m_str, serialization::StringOnlyMessageTag());
                }

              private:
                std::string m_str;
            };
            static const char *identifier();
        };

        class AppStartupToServer {
          public:
            static const char *identifier();
        };
    } // namespace messages

    /// @brief BaseDevice component, to be used only with the "OSVR" special
    /// device.
    class SystemComponent : public DeviceComponent {
      public:
        /// @brief Get the special device name to be used with this component.
        OSVR_COMMON_EXPORT static const char *deviceName();

        OSVR_COMMON_EXPORT SystemComponent();

        /// @brief Message from server to client, replacing all routes.
        MessageRegistration<messages::RoutesFromServer> routesOut;

        OSVR_COMMON_EXPORT void sendRoutes(std::string const &routes);

        /// @brief Message from client to server, notifying of app ID.
        MessageRegistration<messages::AppStartupToServer> appStartup;

      private:
        virtual void m_parentSet();
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_SystemComponent_h_GUID_F74B8728_915D_44AE_612B_6AB934CA4ED2
