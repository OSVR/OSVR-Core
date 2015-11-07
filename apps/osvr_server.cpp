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
#include <osvr/Server/ConfigFilePaths.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

using osvr::server::detail::out;
using osvr::server::detail::err;
using std::endl;

static osvr::server::ServerPtr server;

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    out << "Received shutdown signal..." << endl;
    server->signalStop();
}

int main(int argc, char *argv[]) {
    std::vector<std::string> configPaths;
    if (argc > 1) {
        configPaths = {std::string(argv[1])};
    } else {
        out << "Using default config files - pass a filename on the command "
               "line to use a different one." << endl;
        configPaths = osvr::server::getDefaultConfigFilePaths();
    }

    server = osvr::server::configureServerFromFirstFileInList(configPaths);
    if (!server) {
        out << "Using default config data." << endl;
        server = osvr::server::configureServerFromFile("");
    }
    if (!server) {
        return -1;
    }

    out << "Registering shutdown handler..." << endl;
    osvr::server::registerShutdownHandler<&handleShutdown>();

    out << "Starting server mainloop..." << endl;
    server->startAndAwaitShutdown();

    out << "Server mainloop exited." << endl;

    return 0;
}
