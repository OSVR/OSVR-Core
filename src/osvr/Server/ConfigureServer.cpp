/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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

// Internal Includes
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/Server.h>
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/SearchPath.h>
#include <osvr/Util/Verbosity.h>
#include "JSONResolvePossibleRef.h"

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
    namespace detail {
        class StreamPrefixer {
          public:
            StreamPrefixer(const char *prefix, std::ostream &os)
                : m_prefix(prefix), m_os(&os) {}
            template <typename T> std::ostream &operator<<(T val) {
                return (*m_os) << m_prefix << val;
            }

          private:
            const char *m_prefix;
            std::ostream *m_os;
        };

        static detail::StreamPrefixer out("[OSVR Server] ", std::cout);
        static detail::StreamPrefixer err("[OSVR Server] ", std::cerr);
    } // namespace detail

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

        Json::Value const &getMember(const char *memberName) const {
            return root[memberName];
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
        Json::Value const &root(m_data->root);
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

        m_server->setHardwareDetectOnConnection();

        return m_server;
    }

    static const char PLUGINS_KEY[] = "plugins";
    bool ConfigureServer::loadPlugins() {
        Json::Value const &root(m_data->root);
        const Json::Value plugins = root[PLUGINS_KEY];
        bool success = true;
        for (Json::ArrayIndex i = 0, e = plugins.size(); i < e; ++i) {
            if (!plugins[i].isString()) {
                success = false;
                m_failedPlugins.push_back(std::make_pair(
                    "Plugin entry " + boost::lexical_cast<std::string>(i),
                    "Plugin name not string: " + plugins[i].toStyledString()));
                // skip it!
                continue;
            }

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
        Json::Value const &root(m_data->root);
        Json::Value const &drivers = root[DRIVERS_KEY];
        for (auto const &thisDriver : drivers) {
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
        bool success = false;
        Json::Value const &routes = m_data->getMember(ROUTES_KEY);
        if (routes.isNull()) {
            return success;
        }
        for (Json::ArrayIndex i = 0, e = routes.size(); i < e; ++i) {
            const Json::Value thisRoute = routes[i];
            m_server->addRoute(thisRoute.toStyledString());
            success = true;
        }
        return success;
    }

    static const char ALIASES_KEY[] = "aliases";
    bool ConfigureServer::processAliases() {
        bool success = false;
        Json::Value const &aliases = m_data->getMember(ALIASES_KEY);
        if (aliases.isNull()) {
            return success;
        }
        success = m_server->addAliases(aliases);

        return success;
    }
    static const char EXTERNALDEVICES_KEY[] = "externalDevices";
    static const char DEVICENAME_KEY[] = "deviceName";
    static const char DESCRIPTOR_KEY[] = "descriptor";
    bool ConfigureServer::processExternalDevices() {
        bool success = false;
        Json::Value const &devices = m_data->getMember(EXTERNALDEVICES_KEY);
        if (devices.isNull()) {
            return success;
        }
        for (auto const &devPath : devices.getMemberNames()) {
            auto const &dev = devices[devPath];
            auto const &devName = dev[DEVICENAME_KEY];
            auto const &server = dev[SERVER_KEY];
            auto descriptor = resolvePossibleRef(dev[DESCRIPTOR_KEY]);
            if (devName.isNull()) {
                OSVR_DEV_VERBOSE(
                    "Missing 'deviceName' for external device entry of "
                    << devPath << " : " << dev.toStyledString());
                continue;
            }
            if (server.isNull()) {
                OSVR_DEV_VERBOSE(
                    "Missing 'server' for external device entry of "
                    << devPath << " : " << dev.toStyledString());
                continue;
            }

            if (descriptor.isNull()) {
                OSVR_DEV_VERBOSE("Missing 'descriptor' file or object for "
                                 "external device entry of "
                                 << devPath << " : " << dev.toStyledString());
                continue;
            }

            m_server->addExternalDevice(devPath, devName.asString(),
                                        server.asString(),
                                        descriptor.toStyledString());
            success = true;
        }
        return success;
    }

    static const char DISPLAY_KEY[] = "display";
    static const char DISPLAY_PATH[] = "/display";
    bool ConfigureServer::processDisplay() {
        bool success = false;
        Json::Value const &display = m_data->getMember(DISPLAY_KEY);
        if (display.isNull()) {
            return success;
        }

        auto result = resolvePossibleRef(display);
        if (result.isNull()) {
            OSVR_DEV_VERBOSE(
                "ERROR: Could not load an object or display descriptor "
                "file specified by: "
                << display.toStyledString());
            return success;
        }

        // OK, got it.
        success = m_server->addString(DISPLAY_PATH, result.toStyledString());
        return success;
    }

    void ConfigureServer::loadAutoPlugins() { m_server->loadAutoPlugins(); }

} // namespace server
} // namespace osvr
