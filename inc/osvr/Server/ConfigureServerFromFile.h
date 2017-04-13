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
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/Server.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/LogNames.h>

// Library/third-party includes
// - none

// Standard includes
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

namespace osvr {
namespace server {

    inline const char *getDefaultConfigFilename() {
        return "osvr_server_config.json";
    }

    /// @brief This uses a file name to attempt to configure the server with
    /// that config file.
    /// Pass an empty string to use the default config.
    /// This is the basic common code of a server app's setup, ripped out
    /// of the main server app to make alternate server-acting apps simpler to
    /// develop.
    OSVR_SERVER_EXPORT ServerPtr configureServerFromFile(std::string const &configName);

    OSVR_SERVER_EXPORT ServerPtr configureServerFromString(std::string const &json);

    /// @brief This iterates over a vector that contains a list of potential
    /// config files, and uses the first working one to create the server
    /// instance.
    OSVR_SERVER_EXPORT ServerPtr configureServerFromFirstFileInList(
        std::vector<std::string> const &configNames);

} // namespace server
} // namespace osvr

#endif // INCLUDED_ConfigureServerFromFile_h_GUID_9DA4C152_2AE4_4394_E19E_C0B7EA41804F
