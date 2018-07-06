/** @file
    @brief Header

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
#include <osvr/Server/Server.h>
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Server/ConfigFilePaths.h>
#include <osvr/Util/PlatformConfig.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/LogNames.h>

// Library/third-party includes
// - none

// Standard includes
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

namespace osvr {
namespace server {

    ServerPtr configureServerFromString(std::string const &json) {
        auto log =
            ::osvr::util::log::make_logger(::osvr::util::log::OSVR_SERVER_LOG);

        ServerPtr ret;
        osvr::server::ConfigureServer srvConfig;
        log->info() << "Constructing server as configured...";
        try {
            srvConfig.loadConfig(json);
            ret = srvConfig.constructServer();
        } catch (std::exception &e) {
            log->error()
                << "Caught exception constructing server from JSON config "
                   "file: "
                << e.what();
            return nullptr;
        }

        {
            log->info() << "Loading auto-loadable plugins...";
            srvConfig.loadAutoPlugins();
        }

        {
            log->info() << "Loading plugins...";
            srvConfig.loadPlugins();
            if (!srvConfig.getSuccessfulPlugins().empty()) {
                log->info() << "Successfully loaded the following plugins:";
                for (auto const &plugin : srvConfig.getSuccessfulPlugins()) {
                    log->info() << " - " << plugin;
                }
            }
            if (!srvConfig.getFailedPlugins().empty()) {
                log->warn() << "Failed to load the following plugins:";
                for (auto const &pluginError : srvConfig.getFailedPlugins()) {
                    log->warn() << " - " << pluginError.first << "\t"
                                << pluginError.second;
                }
            }
        }

        {
            log->info() << "Instantiating configured drivers...";
            bool success = srvConfig.instantiateDrivers();
            if (!srvConfig.getSuccessfulInstantiations().empty()) {
                log->info() << "Successes:";
                for (auto const &driver :
                     srvConfig.getSuccessfulInstantiations()) {
                    log->info() << " - " << driver;
                }
            }
            if (!srvConfig.getFailedInstantiations().empty()) {
                log->error() << "Errors:";
                for (auto const &error : srvConfig.getFailedInstantiations()) {
                    log->error()
                        << " - " << error.first << "\t" << error.second;
                }
            }
        }

        if (srvConfig.processExternalDevices()) {
            log->info()
                << "External devices found and parsed from config file.";
        }

        if (srvConfig.processRoutes()) {
            log->info() << "Routes found and parsed from config file.";
        }

        if (srvConfig.processAliases()) {
            log->info() << "Aliases found and parsed from config file.";
        }

        if (srvConfig.processDisplay()) {
            log->info()
                << "Display descriptor found and parsed from config file.";
        } else {
            log->info()
                << "Using OSVR HDK for display configuration. "
                   "Did not find an alternate valid 'display' object in config "
                   "file.";
        }

        if (srvConfig.processRenderManagerParameters()) {
            log->info() << "RenderManager config found and parsed from the "
                           "config file.";
        }

        log->info() << "Triggering automatic hardware detection...";
        ret->triggerHardwareDetect();

        return ret;
    }
    inline void debugDumpSearchPath(::osvr::util::log::LoggerPtr log) {
        log->debug() << "Search path for relative file names:";
        for (auto &path : getConfigDirectories()) {
            log->debug() << path;
        }
    }
    inline ServerPtr
    internalConfigureServerFromFile(std::string const &configName,
                                    ::osvr::util::log::LoggerPtr log) {
        for (auto const &resolvedPath : tryResolvingPath(configName)) {
            log->trace() << "Trying path '" << resolvedPath << "'";
            std::ifstream config(resolvedPath);
            if (!config.good()) {
                log->trace() << "Config file '" << configName
                             << "' not found at " << resolvedPath;
                continue;
            }

            std::stringstream sstr;
            sstr << config.rdbuf();
            return configureServerFromString(sstr.str());
        }
        return nullptr;
    }
    ServerPtr configureServerFromFile(std::string const &configName) {
        auto log =
            ::osvr::util::log::make_logger(::osvr::util::log::OSVR_SERVER_LOG);
        debugDumpSearchPath(log);
        log->info() << "Attempting to load config file '" << configName << "'.";
        auto ret = internalConfigureServerFromFile(configName, log);
        if (ret) {
            return ret;
        }
        log->error() << "Config file '" << configName
                     << "' not found in any of the config search paths.";
        return nullptr;
    }

    ServerPtr configureServerFromFirstFileInList(
        std::vector<std::string> const &configNames) {
        auto log =
            ::osvr::util::log::make_logger(::osvr::util::log::OSVR_SERVER_LOG);
        debugDumpSearchPath(log);
        for (const auto &name : configNames) {
            auto ret = internalConfigureServerFromFile(name, log);
            if (ret) {
                return ret;
            }
        }
        log->error() << "Could not find a valid config file!";
        return nullptr;
    }

} // namespace server
} // namespace osvr
