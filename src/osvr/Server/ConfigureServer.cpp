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
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/Server.h>
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/SearchPath.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>                 // for stem()
#include <boost/algorithm/string/predicate.hpp> // for iends_with()

// Standard includes
#include <stdexcept>
#include <iostream>
#include <vector>

namespace osvr {
namespace server {
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

    static const char SERVER_KEY[] = "server";
    static const char INTERFACE_KEY[] = "interface";
    static const char LOCAL_KEY[] = "local";
    static const char PORT_KEY[] = "port"; // not the triwizard cup.
    static const char SLEEP_KEY[] = "sleep";

    ServerPtr ConfigureServer::constructServer() {
        Json::Value &root(m_data->root);
        bool local = true;
        std::string iface;
        boost::optional<int> port;
        int sleepTime = 1000; // microseconds

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

            Json::Value jsonSleepTime = jsonServer[SLEEP_KEY];
            if (jsonSleepTime.isDouble()) {
                // Sleep time is in milliseconds in the config file.
                // Convert to microseconds for internal use.
                sleepTime = static_cast<int>(jsonSleepTime.asDouble() * 1000.0);
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

        if (sleepTime > 0.0)
            m_server->setSleepTime(sleepTime);

        return m_server;
    }

    bool ConfigureServer::loadPlugins() {

        bool success = true;
        // find path where all plugins are hiding
        pluginhost::SearchPath pluginPath(pluginhost::getPluginSearchPath());
        pluginhost::FileList filesPaths =
            pluginhost::getAllFilesWithExt(pluginPath, OSVR_PLUGIN_EXTENSION);

        for (const auto &plugin : filesPaths) {
            if (boost::iends_with(boost::filesystem::path(plugin).stem().generic_string(),
                                  OSVR_PLUGIN_IGNORE_SUFFIX)) {
                std::cout << "Ignoring plugin marked for manual loading " << boost::filesystem::path(plugin).stem().generic_string() << "..." << std::endl;
                continue;
            }

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

    ConfigureServer::SuccessList const &
    ConfigureServer::getSuccessfulPlugins() const {
        return m_successfulPlugins;
    }

    ConfigureServer::ErrorList const &
    ConfigureServer::getFailedPlugins() const {
        return m_failedPlugins;
    }

    static const char DRIVERS_KEY[] = "drivers";
    static const char DRIVER_KEY[] = "driver";
    static const char PLUGIN_KEY[] = "plugin";
    static const char PARAMS_KEY[] = "params";
    bool ConfigureServer::instantiateDrivers() {
        bool success = true;
        Json::Value &root(m_data->root);
        const Json::Value drivers = root[DRIVERS_KEY];
        for (Json::ArrayIndex i = 0, e = drivers.size(); i < e; ++i) {
            const Json::Value thisDriver = drivers[i];
            const bool hasPlugin = thisDriver[PLUGIN_KEY].isString();
            const bool hasDriver = thisDriver[DRIVER_KEY].isString();
            if (!hasPlugin && !hasDriver) {
                success = false;
                m_failedInstances.push_back(std::make_pair(
                    "?", "Entry present in drivers but lacking both a driver "
                         "name and a plugin name"));
                // Skip this one.
                continue;
            }
            if (!hasPlugin) {
                success = false;
                m_failedInstances.push_back(
                    std::make_pair("?/" + thisDriver[DRIVER_KEY].asString(),
                                   "Entry present in drivers with a driver "
                                   "name but lacking a plugin name"));
                // Skip this one.
                continue;
            }

            const std::string plugin = thisDriver[PLUGIN_KEY].asString();

            if (!hasPlugin) {
                success = false;
                m_failedInstances.push_back(std::make_pair(
                    plugin + "/?", "Entry present in drivers with a plugin "
                                   "name but lacking a driver name"));
                // Skip this one.
                continue;
            }

            const std::string driver = thisDriver[DRIVER_KEY].asString();

            try {
                m_server->instantiateDriver(
                    plugin, driver, thisDriver[PARAMS_KEY].toStyledString());

                m_successfulInstances.push_back(plugin + "/" + driver);
            } catch (std::exception &e) {
                m_failedInstances.push_back(
                    std::make_pair(plugin + "/" + driver, e.what()));
                success = false;
            }
        }
        return success;
    }

    ConfigureServer::SuccessList const &
    ConfigureServer::getSuccessfulInstantiations() const {
        return m_successfulInstances;
    }

    ConfigureServer::ErrorList const &
    ConfigureServer::getFailedInstantiations() const {
        return m_failedInstances;
    }

    static const char ROUTES_KEY[] = "routes";
    bool ConfigureServer::processRoutes() {
        Json::Value &root(m_data->root);
        bool success = false;
        const Json::Value routes = root[ROUTES_KEY];
        for (Json::ArrayIndex i = 0, e = routes.size(); i < e; ++i) {
            const Json::Value thisRoute = routes[i];
            m_server->addRoute(thisRoute.toStyledString());
            success = true;
        }
        return success;
    }

} // namespace server
} // namespace osvr
