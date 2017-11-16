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
#include <osvr/Server/ConfigFilePaths.h>
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Server/RegisterShutdownHandler.h>
#include <osvr/Util/LogNames.h>
#include <osvr/Util/LogRegistry.h>
#include <osvr/Util/Logger.h>

// Library/third-party includes
#include <boost/program_options.hpp>

// Standard includes
#include <iostream>
#include <vector>

namespace opt = boost::program_options;

static osvr::server::ServerPtr server;
using ::osvr::util::log::OSVR_SERVER_LOG;

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    auto log = ::osvr::util::log::make_logger(OSVR_SERVER_LOG);
    log->info() << "Received shutdown signal...";
    server->signalStop();
}

int main(int argc, char *argv[]) {
    auto log = ::osvr::util::log::make_logger(OSVR_SERVER_LOG);

    std::vector<std::string> configPaths;

    opt::options_description optionsAll("All Options");
    opt::options_description optionsVisible("Command Line Options");
    opt::positional_options_description optionsPositional;

    optionsPositional.add("config", -1);
    // clang-format off
    optionsVisible.add_options()
        ("config", opt::value<std::vector<std::string> >(),
            "server configuration filename (can also pass without a flag as a positional option)")
        ("help,h", "display this help message")
        ("verbose,v", "enable verbose logging")
        ("debug,d", "enable debug logging");
    // clang-format on
    optionsAll.add(optionsVisible);

    opt::variables_map values;
    try {
        opt::store(opt::command_line_parser(argc, argv)
                       .options(optionsAll)
                       .positional(optionsPositional)
                       .run(),
                   values);
        opt::notify(values);
    } catch (opt::invalid_command_line_syntax &e) {
        log->error() << e.what();
        return 1;
    } catch (opt::unknown_option &e) {
        log->error() << e.what(); // may want to replace boost msg
        return 1;
    }

    if (values.count("help")) {
        std::cout << optionsVisible << std::endl;
        return 0;
    }
    {
        using namespace osvr::util::log;
        if (values.count("debug")) {
            LogRegistry::instance().setLevel(LogLevel::trace);
            LogRegistry::instance().setConsoleLevel(LogLevel::trace);
            log->trace("Debug logging enabled.");
        } else if (values.count("verbose")) {
            LogRegistry::instance().setLevel(LogLevel::debug);
            LogRegistry::instance().setConsoleLevel(LogLevel::debug);
            log->debug("Verbose logging enabled.");
        }
    }

    if (values.count("config")) {
        std::string configFileArgument =
            values["config"].as<std::vector<std::string> >().front();
        log->info() << "Using config file " << configFileArgument
                    << " from command line argument.";
        configPaths = {configFileArgument};
    } else {
        log->info()
            << "Using default config file - pass a filename on the command "
               "line to use a different one.";
        configPaths = osvr::server::getDefaultConfigFilePaths();
    }

    server = osvr::server::configureServerFromFirstFileInList(configPaths);
    if (!server) {
        // only attempt to load the empty config if no arguments are passed.
        if (!values.count("config")) {
            log->info() << "Could not find a valid config file in the default "
                           "search paths. Using default config object.";
            server = osvr::server::configureServerFromString("{ }");
        } else {
            return -1;
        }
    }

    if (!server) {
        log->error() << "Unknown error while creating server.";
        return -1;
    }

    log->info() << "Registering shutdown handler...";
    osvr::server::registerShutdownHandler<&handleShutdown>();

    log->info() << "Starting server mainloop: OSVR Server is ready to go!";
    server->startAndAwaitShutdown();

    log->info() << "OSVR Server exited.";

    return 0;
}
