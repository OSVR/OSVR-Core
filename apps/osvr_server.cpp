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

// Library/third-party includes
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/optional.hpp>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using osvr::server::detail::out;
using osvr::server::detail::err;
using std::endl;
namespace opt = boost::program_options;
namespace fs = boost::filesystem;

static osvr::server::ServerPtr server;

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    out << "Received shutdown signal..." << endl;
    server->signalStop();
}

int main(int argc, char *argv[]) {
    const std::string helpOpt("help");
    const std::string configOpt("option");

    std::string configName;

    opt::options_description optionsAll("All Options");
    opt::options_description optionsVisible("Command Line Options");
    opt::positional_options_description optionsPositional;

    optionsPositional.add(configOpt.c_str(), -1);
    optionsAll.add_options()(
        configOpt.c_str(),
        opt::value<std::string>(&configName)
            ->default_value(osvr::server::getDefaultConfigFilename()),
        "override config filename");
    optionsVisible.add_options()(helpOpt.c_str(), "display this help message");
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
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (opt::unknown_option &e) {
        std::cerr << e.what() << std::endl; // may want to replace boost msg
        return 1;
    }

    if (values.count(helpOpt)) {
        std::cout << optionsVisible << std::endl;
        return 0;
    }

    configName = values[configOpt].as<std::string>();

    boost::optional<fs::path> configPath(configName);
    try {
        if (!fs::exists(*configPath)) {
            out << "File '" << configName
                << "' not found.  Using blank config" << endl;
            configPath = boost::none;
        } else {
            if (fs::is_directory(*configPath)) {
                err << "'" << configName << "' is a directory" << endl;
                return -1;
            } else if (!fs::is_regular_file(*configPath)) {
                err << "'" << configName << "' is special file" << endl;
                return -1;
            }
        }
    } catch (fs::filesystem_error &e) {
        err << "Could not open config file at '" << configName << "'"
            << endl;
        err << "Reason " << e.what() << endl;
        configPath = boost::none;
    }
    
    if (configPath) {
        server = osvr::server::configureServerFromFile(configName);
    } else {
        server = osvr::server::configureServerFromString("{ }");
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
