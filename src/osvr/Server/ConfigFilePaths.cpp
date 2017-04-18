/** @file
@brief Platform dependent search paths for osvr server config files.

@date 2017

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
#include <osvr/Util/PlatformConfig.h>
#include <osvr/Server/ConfigFilePaths.h>
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Util/GetEnvironmentVariable.h>

// Library/third-party includes
#include <boost/filesystem.hpp>

// Standard includes
#include <vector>

namespace osvr {
    namespace server {

        inline std::string getUserConfigDirectory() {
            namespace fs = boost::filesystem;
            using osvr::util::getEnvironmentVariable;

            fs::path configDir;
            std::string configSubpath = "config";

#if defined(OSVR_LINUX)
            // $XDG_CONFIG_HOME defines the base directory relative to which user
            // specific non-essential data files should be stored. If
            // $XDG_CONFIG_HOME is either not set or empty, a default equal to
            // $HOME/.config should be used.
            auto xdg_cache_dir = getEnvironmentVariable("XDG_CONFIG_HOME");
            if (xdg_cache_dir) {
                configDir = *xdg_cache_dir;
            }
            else {
                auto home_dir = getEnvironmentVariable("HOME");
                configDir = fs::path(*home_dir) / ".config";
            }
            configDir /= fs::path("osvr");
#elif defined(OSVR_MACOSX)
            auto home_dir = getEnvironmentVariable("HOME");
            if (home_dir) {
                configDir = *home_dir;
            }
            configDir /= "Library" / fs::path("Application Support") / fs::path("OSVR") / configSubpath;
#elif defined(OSVR_WINDOWS)
            /// @todo there's actually a win32 api call to get localappdata
            /// that's preferred to the env var.
            auto local_app_dir = getEnvironmentVariable("LocalAppData");
            if (local_app_dir) {
                configDir = *local_app_dir;
            }
            else {
                configDir = "c:/";
            }
            configDir /= fs::path("OSVR") / configSubpath;
#endif

            return configDir.string();
        }

        std::vector<std::string> getDefaultConfigFilePaths() {
            namespace fs = boost::filesystem;
            std::vector<std::string> names;
            std::string configFileName = getDefaultConfigFilename();

            fs::path userConfigDirectory(getUserConfigDirectory());
            auto userConfig = userConfigDirectory / configFileName;

            names.push_back(userConfig.string());
            names.push_back(configFileName);

            return names;
        }
    }
}