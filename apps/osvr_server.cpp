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
    const std::string configOpt("config");

    std::string configName;

    opt::options_description options("OSVR Server Options");
    options.add_options()(
        configOpt.c_str(),
        opt::value<std::string>(&configName)
            ->default_value(osvr::server::getDefaultConfigFilename()),
        "override config filename")(helpOpt.c_str(),
                                    "display this help message");

    opt::variables_map values;
    try {
        opt::store(opt::parse_command_line(argc, argv, options), values);
        opt::notify(values);
    } catch (opt::invalid_command_line_syntax &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (values.count(helpOpt)) {
        std::cout << options << std::endl;
        return 1;
    }

    if (!values.count(configOpt)) {
        out << "Using default config file - pass a filename with --config= "
               "to use a different one."
            << endl;
    } else {
        configName = values[configOpt].as<std::string>();
    }

    fs::path configPath(configName);
    if (!fs::exists(configPath)) {
        out << "Creating blank config at \"" << configName << "\"" << endl;
        fs::ofstream configOut{configPath};
        configOut << "{ }\n";
        configOut.close();
    } else {
        if (fs::is_directory(configPath)) {
            err << "\"" << configName << "\" is a directory" << endl;
            return -1;
        } else if (!fs::is_regular_file(configPath)) {
            err << "\"" << configName << "\" is special file" << endl;
            return -1;
        }
    }

    server = osvr::server::configureServerFromFile(configName);
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
