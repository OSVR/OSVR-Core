/** @file
    @brief Platform specific search paths for osvr server config files.

    @date 2017

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2017 Sensics, Inc.
// Copyright 2018 Collabora
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

#ifndef INCLUDED_ConfigFilePaths_h_GUID_241E9C9C_0E0E_46B0_9DED_8F8059306192
#define INCLUDED_ConfigFilePaths_h_GUID_241E9C9C_0E0E_46B0_9DED_8F8059306192

// Internal Includes
#include <osvr/Server/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <string>

namespace osvr {
namespace server {

    /// @brief Get list of directories to search for config files.
    OSVR_SERVER_EXPORT std::vector<std::string> getConfigDirectories();

    /// @brief this returns a vector of default server configuration file paths.
    OSVR_SERVER_EXPORT std::vector<std::string> getDefaultConfigFilePaths();

    /// @brief given a input filename/path, returns potential full paths from
    /// combining relative input paths with potential config file locations.
    OSVR_SERVER_EXPORT std::vector<std::string>
    tryResolvingPath(std::string const &inputFilename);

} // namespace server
} // namespace osvr
#endif // INCLUDED_ConfigFilePaths_h_GUID_241E9C9C_0E0E_46B0_9DED_8F8059306192
