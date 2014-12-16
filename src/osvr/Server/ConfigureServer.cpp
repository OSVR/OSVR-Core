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
    static const char PLUGINS_KEY[] = "plugins";

    class ConfigureServerData : boost::noncopyable {
      public:
        template <typename T> inline void parse(T &json) {
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(json, root);
            if (!parsingSuccessful) {
                throw std::runtime_error("Error in parsing JSON: " +
                                         reader.getFormattedErrorMessages());
            }
        }

        Json::Value root;
    };

    ConfigureServer::ConfigureServer() : m_data(new ConfigureServerData()) {}

    /// Must be in implementation file to handle deletion of private data class.
    ConfigureServer::~ConfigureServer() {}

    void ConfigureServer::loadConfig(std::string const &json) {
        m_data->parse(json);
    }

    void ConfigureServer::loadConfig(std::istream &json) {
        m_data->parse(json);
    }

    ServerPtr ConfigureServer::constructServer() {
        Json::Value &root(m_data->root);
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
            m_server = Server::createLocal();
        } else {
            connection::ConnectionPtr connPtr(
                connection::Connection::createSharedConnection(iface, port));
            m_server = Server::create(connPtr);
        }
        return m_server;
    }

    bool ConfigureServer::loadPlugins() {
        Json::Value &root(m_data->root);
        const Json::Value plugins = root[PLUGINS_KEY];
        bool success = true;
        for (Json::ArrayIndex i = 0, e = plugins.size(); i < e; ++i) {
            const std::string plugin = plugins[i].asString();
            try {
                m_server->loadPlugin(plugin);
                m_successfulPlugins.push_back(plugin);
            } catch (std::exception &e) {
                m_failedPlugins.push_back(std::make_pair(plugin, e.what()));
                success = false;
            }
        }
        return success;
    }

    ConfigureServer::PluginList const &
    ConfigureServer::getSuccessfulPlugins() const {
        return m_successfulPlugins;
    }

    ConfigureServer::PluginErrorList const &
    ConfigureServer::getFailedPlugins() const {
        return m_failedPlugins;
    }

} // namespace server
} // namespace osvr
