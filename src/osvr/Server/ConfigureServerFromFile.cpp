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

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

namespace osvr {
namespace server {


    ServerPtr configureServerFromFile(std::string const &configName) {
        using detail::out;
        using detail::err;
        using std::endl;
        ServerPtr ret;
        osvr::server::ConfigureServer srvConfig;

        if(!configName.empty()) {
            out << "Using config file '" << configName << "'" << endl;
            std::ifstream config(configName);
            if (!config.good()) {
                err << "\n"
                    << "Could not open config file!" << endl;
                err << "Searched in the current directory; file may be "
                       "misspelled, missing, or in a different directory." << endl;
                return nullptr;
            }
            try {
                srvConfig.loadConfig(config);
            } catch (std::exception &e) {
                err << "Caught exception attempting to parse server JSON config file: " << e.what() << endl;
                return nullptr;
            }
        } else {
            srvConfig.loadConfig("{}");
        }
        try {
            ret = srvConfig.constructServer();
        } catch (std::exception &e) {
            err << "Caught exception constructing server from JSON config "
                   "file: " << e.what() << endl;
            return nullptr;
        }

        {
            out << "Loading auto-loadable plugins..." << endl;
            srvConfig.loadAutoPlugins();
        }

        {
            out << "Loading plugins..." << endl;
            srvConfig.loadPlugins();
            if (!srvConfig.getSuccessfulPlugins().empty()) {
                out << "Successful plugins:" << endl;
                for (auto const &plugin : srvConfig.getSuccessfulPlugins()) {
                    out << " - " << plugin << endl;
                }
                out << "\n";
            }
            if (!srvConfig.getFailedPlugins().empty()) {
                out << "Failed plugins:" << endl;
                for (auto const &pluginError : srvConfig.getFailedPlugins()) {
                    out << " - " << pluginError.first << "\t"
                        << pluginError.second << endl;
                }
                out << "\n";
            }

            out << "\n";
        }

        {
            out << "Instantiating configured drivers..." << endl;
            bool success = srvConfig.instantiateDrivers();
            if (!srvConfig.getSuccessfulInstantiations().empty()) {
                out << "Successes:" << endl;
                for (auto const &driver :
                     srvConfig.getSuccessfulInstantiations()) {
                    out << " - " << driver << endl;
                }
                out << "\n";
            }
            if (!srvConfig.getFailedInstantiations().empty()) {
                out << "Errors:" << endl;
                for (auto const &error : srvConfig.getFailedInstantiations()) {
                    out << " - " << error.first << "\t" << error.second << endl;
                }
                out << "\n";
            }
            out << "\n";
        }

        if (srvConfig.processExternalDevices()) {
            out << "External devices found and parsed from config file."
                << endl;
        }

        if (srvConfig.processRoutes()) {
            out << "Routes found and parsed from config file." << endl;
        }

        if (srvConfig.processAliases()) {
            out << "Aliases found and parsed from config file." << endl;
        }

        if (srvConfig.processDisplay()) {
            out << "Display descriptor found and parsed from config file"
                << endl;
        } else {
            out << "No valid 'display' object found in config file - server "
                   "may use the OSVR HDK as a default." << endl;
        }

        if (srvConfig.processRenderManagerParameters()) {
            out << "RenderManager config found and parsed from the config file"
                << endl;
        }

        out << "Triggering a hardware detection..." << endl;
        ret->triggerHardwareDetect();

        return ret;
    }

    ServerPtr configureServerFromFirstFileInList(std::vector<std::string> const &configNames) {
        using detail::out;
        using detail::err;
        using std::endl;
        for(const auto name: configNames) {
            std::ifstream config(name);
            if(config.good()) {
                return configureServerFromFile(name);
            }
        }
        err << "Could not open config file!" << endl;
        err << "Attempted the following files:" << endl;
        for (auto i = configNames.begin(); i != configNames.end(); i++) {
            std::cout << *i << std::endl;
        }
        return nullptr;
    }

} // namespace server
} // namespace osvr
