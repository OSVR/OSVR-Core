/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

/// @todo Merge this file back into the main cpp

// Internal Includes
#include "ServerImpl.h"
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Util/MessageKeys.h>
#include <osvr/Connection/MessageType.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <functional>
#include <algorithm>

namespace osvr {
namespace server {

    bool ServerImpl::addRoute(std::string const &routingDirective) {
        bool wasNew;
        m_callControlled([&] { wasNew = m_routes.addRoute(routingDirective); });
        return wasNew;
    }

    std::string ServerImpl::getRoutes(bool styled) const {
        std::string ret;
        m_callControlled([&] { ret = m_routes.getRoutes(styled); });
        return ret;
    }

    void ServerImpl::m_sendRoutes() {
        std::string message = getRoutes(false);
        OSVR_DEV_VERBOSE("Transmitting " << m_routes.size()
                                         << " routes to the client.");
        m_sysDevice->sendData(m_routingMessageType.get(), message.c_str(),
                              message.size());
    }
} // namespace server
} // namespace osvr