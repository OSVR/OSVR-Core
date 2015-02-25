/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/SystemComponent.h>
#include <osvr/Util/MessageKeys.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        const char *RoutesFromServer::identifier() {
            return util::messagekeys::routingData();
        }

        const char *AppStartupToServer::identifier() {
            return "com.osvr.system.appstartup";
        }

    } // namespace messages

    const char *SystemComponent::deviceName() {
        return util::messagekeys::systemSender();
    }

    SystemComponent::SystemComponent() {}

    void SystemComponent::m_parentSet() {
        m_getParent().registerMessageType(routesOut);
        m_getParent().registerMessageType(appStartup);
    }
} // namespace common
} // namespace osvr