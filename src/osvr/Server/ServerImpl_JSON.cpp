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
#include <algorithm>

namespace osvr {
namespace server {
    static inline Json::Value
    parseRoutingDirective(std::string const &routingDirective) {
        Json::Reader reader;
        Json::Value val;
        if (!reader.parse(routingDirective, val)) {
            throw std::runtime_error("Invalid JSON routing directive: " +
                                     routingDirective);
        }
        return val;
    }
    static const char DESTINATION_KEY[] = "destination";

    void ServerImpl::addRoute(std::string const &routingDirective) {
        Json::Value newDirective = parseRoutingDirective(routingDirective);
        std::string destination = newDirective[DESTINATION_KEY].asString();

        bool replaced = false;
        std::replace_if(
            begin(m_routingDirectives),
            end(m_routingDirectives), [&](std::string const &directive) {
            Json::Value candidate = parseRoutingDirective(directive);
            bool match = (candidate[DESTINATION_KEY].asString() == destination);
            if (match) {
                replaced = true;
            }
            return match;
        }, routingDirective);
        if (!replaced) {
            m_routingDirectives.push_back(routingDirective);
        }
    }

    std::string ServerImpl::getRoutes(bool styled) const {
        Json::Value routes(Json::arrayValue);
        for (auto const &r : m_routingDirectives) {
            routes.append(parseRoutingDirective(r));
        }
        std::string ret;
        if (styled) {
            ret = routes.toStyledString();
        } else {
            Json::FastWriter writer;
            ret = writer.write(routes);
        }
        return ret;
    }

    void ServerImpl::m_sendRoutes() {
        std::string message = getRoutes(false);
        OSVR_DEV_VERBOSE("Transmitting " << m_routingDirectives.size()
                                         << " routes to the client.");
        m_sysDevice->sendData(m_routingMessageType.get(), message.c_str(),
                              message.size());
    }
} // namespace server
} // namespace osvr