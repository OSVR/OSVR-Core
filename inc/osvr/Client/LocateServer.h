/** @file
    @brief Internal API to locate the current OSVR Server on the file system.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_LocateServer_h_GUID_7D10F12E_2A83_48BB_7581_ED7A7F5188AC
#define INCLUDED_LocateServer_h_GUID_7D10F12E_2A83_48BB_7581_ED7A7F5188AC


// Internal Includes
#include <osvr/Common/GetEnvironmentVariable.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
    namespace client {

        /** @brief INTERNAL ONLY - get the current server directory, if available. */
        inline boost::optional<std::string> getServerBinaryDirectoryPath() {
            auto server = osvr::common::getEnvironmentVariable("OSVR_SERVER_ROOT");
            return server;
        }

        /** @brief INTERNAL ONLY - get the path to the server executable, if available. */
        inline boost::optional<std::string> getServerBinaryPath() {
            auto binPath = getServerBinaryDirectoryPath();
            if (binPath) {
#if defined(OSVR_WINDOWS)
                return *binPath + "\\osvr_server.exe";
#else
                return *binPath + "/osvr_server";
#endif
            }
            return boost::none;
        }

        /** @brief INTERNAL ONLY - get the path to the server launcher executable, if available. */
        inline boost::optional<std::string> getServerLauncherBinaryPath() {
            return getServerBinaryPath();
        }
    }
}

#endif // INCLUDED_LocateServer_h_GUID_7D10F12E_2A83_48BB_7581_ED7A7F5188AC

