/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/ClientKit/ServerAutoStartC.h>
#include <osvr/Util/ProcessUtils.h>
#include <osvr/Util/PlatformConfig.h>
#include <osvr/Client/LocateServer.h>
#include <osvr/Common/GetEnvironmentVariable.h>

// Library/third-party includes
// - none

// Standard includes
// - none

void osvrClientAttemptServerAutoStart()
{
    // @todo start the server.
#if defined(OSVR_ANDROID)
    // @todo implement auto-start for android
#else
    auto server = osvr::client::getServerBinaryDirectoryPath();
    if (server) {
        OSVR_DEV_VERBOSE("Attempting to auto-start OSVR server from path " << *server);
        auto exePath = osvr::client::getServerLauncherBinaryPath();
        if (!exePath) {
            OSVR_DEV_VERBOSE("No server launcher binary available.");
            return;
        }

        if (osvrStartProcess(exePath->c_str(), server->c_str()) == OSVR_RETURN_FAILURE) {
            OSVR_DEV_VERBOSE("Could not auto-start server process.");
            return;
        }
    } else {
        OSVR_DEV_VERBOSE("The current server location is currently unknown. Assuming server is already running.");
    }
#endif
    return;
}

void osvrClientReleaseAutoStartedServer()
{
#if defined(OSVR_ANDROID)
    // @todo cleanup server thread
#else
    // no-op. Leave the server running.
#endif
    return;
}
