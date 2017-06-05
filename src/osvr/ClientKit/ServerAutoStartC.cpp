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
#include <osvr/Util/GetEnvironmentVariable.h>
#if defined(OSVR_ANDROID)
#include <osvr/Server/ConfigFilePaths.h>
#include <osvr/Server/ConfigureServerFromFile.h>
#endif

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <vector>

// @todo use a thread-safe lazy-initialized singleton pattern
#if defined(OSVR_ANDROID)
static osvr::server::ServerPtr gServer;
#endif

void osvrClientAttemptServerAutoStart()
{
    // @todo start the server.
#if defined(OSVR_ANDROID)
    if(!gServer) {
        try {
            OSVR_DEV_VERBOSE("Creating android auto-start server. Looking at config file paths:");
            std::vector<std::string> configPaths = osvr::server::getDefaultConfigFilePaths();
            for(size_t i = 0; i < configPaths.size(); i++) {
                OSVR_DEV_VERBOSE(configPaths[i]);
            }
            
            gServer = osvr::server::configureServerFromFirstFileInList(configPaths);
            if(!gServer) {
                OSVR_DEV_VERBOSE("Failed to create Android-auto-start server. Most likely the server is already running.");
                return;
            }
            OSVR_DEV_VERBOSE("Android-auto-start server created. Starting server thread...");

            gServer->start();
            OSVR_DEV_VERBOSE("Android server thread started...");
        } catch(...) {
            OSVR_DEV_VERBOSE("Android server auto-start failed to start. Most likely the server is already running.");
            gServer = nullptr;
        }
    }
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
    if(gServer) {
        gServer->stop();
        gServer = nullptr;
    }
#else
    // no-op. Leave the server running.
#endif
    return;
}
