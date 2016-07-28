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
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Server/RegisterShutdownHandler.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/LogNames.h>

// Library/third-party includes
// - none

// Standard includes
#include <exception>
#include <fstream>
#include <iostream>

static osvr::server::ServerPtr server;
using ::osvr::util::log::OSVR_SERVER_LOG;

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    auto log = ::osvr::util::log::make_logger(OSVR_SERVER_LOG);
    log->info() << "Received shutdown signal...";
    server->signalStop();
}

int main(int argc, char *argv[]) {
    ::osvr::util::log::tryInitializingLoggingWithBaseName("osvr_server");
    auto log = ::osvr::util::log::make_logger(OSVR_SERVER_LOG);

    std::string configName(osvr::server::getDefaultConfigFilename());
    if (argc > 1) {
        configName = argv[1];
    } else {
        log->info()
            << "Using default config file - pass a filename on the command "
               "line to use a different one.";
    }

    server = osvr::server::configureServerFromFile(configName);
    if (!server) {
        return -1;
    }

    log->info() << "Registering shutdown handler...";
    osvr::server::registerShutdownHandler<&handleShutdown>();

    log->info() << "Starting server mainloop: OSVR Server is ready to go!";
    server->startAndAwaitShutdown();

    log->info() << "OSVR Server exited.";

    return 0;
}
