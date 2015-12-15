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

#ifndef INCLUDED_ConfigureServerFromFile_h_GUID_9DA4C152_2AE4_4394_E19E_C0B7EA41804F
#define INCLUDED_ConfigureServerFromFile_h_GUID_9DA4C152_2AE4_4394_E19E_C0B7EA41804F

// Internal Includes
#include <osvr/Server/Server.h>
#include <osvr/Server/ConfigureServer.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

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

    inline const char *getDefaultConfigFilename() {
        return "osvr_server_config.json";
    }

    /// @brief This is the basic common code of a server app's setup, ripped out
    /// of the main server app to make alternate server-acting apps simpler to
    /// develop.
    inline ServerPtr configureServerFromFile(std::string const &configName) {
        using detail::out;
        using detail::err;
        using std::endl;
        ServerPtr ret;
        out << "Using config file '" << configName << "'" << endl;
        std::ifstream config(configName);
        if (!config.good()) {
            err << "\n"
                << "Could not open config file!" << endl;
            err << "Searched in the current directory; file may be "
                   "misspelled, missing, or in a different directory."
                << endl;
            return nullptr;
        }

        osvr::server::ConfigureServer srvConfig;
        out << "Constructing server as configured..." << endl;
        try {
            srvConfig.loadConfig(config);
            ret = srvConfig.constructServer();
        } catch (std::exception &e) {
            err << "Caught exception constructing server from JSON config "
                   "file: "
                << e.what() << endl;
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
                out << "Successfully loaded the following plugins:" << endl;
                for (auto const &plugin : srvConfig.getSuccessfulPlugins()) {
                    out << " - " << plugin << endl;
                }
                out << "\n";
            }
            if (!srvConfig.getFailedPlugins().empty()) {
                out << "Failed to load the following plugins:" << endl;
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
            out << "Display descriptor found and parsed from config file."
                << endl;
        } else {
            out << "Using OSVR HDK for display configuration. "
                   "Did not find an alternate valid 'display' object in config "
                   "file."
                << endl;
        }

        if (srvConfig.processRenderManagerParameters()) {
            out << "RenderManager config found and parsed from the config file."
                << endl;
        }

        out << "Triggering automatic hardware detection..." << endl;
        ret->triggerHardwareDetect();

        return ret;
    }
} // namespace server
} // namespace osvr

#endif // INCLUDED_ConfigureServerFromFile_h_GUID_9DA4C152_2AE4_4394_E19E_C0B7EA41804F
