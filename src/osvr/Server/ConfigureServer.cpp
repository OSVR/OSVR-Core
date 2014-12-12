/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/Server.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/optional.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace server {
    static const char SERVER_KEY[] = "server";
    static const char INTERFACE_KEY[] = "interface";
    static const char LOCAL_KEY[] = "local";
    static const char PORT_KEY[] = "port"; // not the triwizard cup.

    static inline Json::Value parse(std::string const &json) {
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(json, root);
        if (!parsingSuccessful) {
            throw std::runtime_error("Error in parsing JSON: " +
                                     reader.getFormattedErrorMessages());
        }
        return root;
    }

    static ServerPtr configuredConstruction(Json::Value &root) {
        ServerPtr ret; // Single return value for NRVO
        bool local = true;
        std::string iface;
        boost::optional<int> port;

        /// Extract data from the JSON structure.
        if (root.isMember(SERVER_KEY)) {
            /// @todo Detect/report invalid or contradictory options here.
            Json::Value jsonServer = root[SERVER_KEY];
            Json::Value jsonInterface = jsonServer[INTERFACE_KEY];
            Json::Value jsonLocal = jsonServer[LOCAL_KEY];
            if (jsonInterface.isString()) {
                local = false;
                iface = jsonInterface.asString();
            } else if (jsonLocal.isBool()) {
                local = jsonLocal.asBool();
            }

            Json::Value jsonPort = jsonServer[PORT_KEY];
            if (jsonPort.isInt()) {
                int myPort = jsonPort.asInt();
                if (myPort < 1) {
                    throw std::out_of_range("Invalid port value: must be >= 1 "
                                            "and for a non-admin execution, "
                                            ">1024");
                }
                port = myPort;
            }
        }

        /// Construct a server, or a connection then a server, based on the
        /// configuration we've extracted.
        if (local && !port) {
            ret = Server::createLocal();
        } else {
            connection::ConnectionPtr connPtr(
                connection::Connection::createSharedConnection(iface, port));
            ret = Server::create(connPtr);
        }
        return ret;
    }

    ServerPtr configuredConstruction(std::string const &json) {
        Json::Value root = parse(json);
        return configuredConstruction(root);
    }

} // namespace server
} // namespace osvr
