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

// Internal Includes
#include "ServerImpl.h"
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Util/MessageKeys.h>
#include <osvr/Connection/MessageType.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

// Standard includes
#include <stdexcept>
#include <functional>

namespace osvr {
namespace server {

    void ServerImpl::m_sendRoutes() {
        Json::Value routes(Json::arrayValue);
        for (auto const &r : m_routingDirectives) {
            Json::Reader reader;
            Json::Value val;
            if (!reader.parse(r, val)) {
                throw std::runtime_error("Invalid JSON routing directive: " +
                                         r);
            }
            routes.append(val);
        }
        Json::FastWriter writer;
        std::string message = writer.write(routes);

        OSVR_DEV_VERBOSE("Transmitting " << m_routingDirectives.size()
                                         << " routes to the client.");
        m_sysDevice->sendData(m_routingMessageType.get(), message.c_str(),
                              message.size());
    }
} // namespace server
} // namespace osvr