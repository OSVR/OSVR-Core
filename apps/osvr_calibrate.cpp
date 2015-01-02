/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Server/RegisterShutdownHandler.h>

// Library/third-party includes
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using std::cout;
using std::cerr;
using std::endl;

static osvr::server::ServerPtr server;

auto SETTLE_TIME = boost::posix_time::seconds(2);

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    cout << "Received shutdown signal..." << endl;
    server->signalStop();
}

int main(int argc, char *argv[]) {
    std::string configName;
    std::string outputName;
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("route", po::value<std::string>(), "route to calibrate")
        ("output,O", po::value<std::string>(&outputName)->default_value(std::string("calibration.json")), "output file")
        ;
    po::options_description hidden("Hidden (positional-only) options");
    hidden.add_options()
        ("config", po::value<std::string>(&configName)->default_value(std::string(osvr::server::getDefaultConfigFilename())))
        ;
    // clang-format on

    po::positional_options_description p;
    p.add("config", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(po::options_description().add(desc).add(hidden))
                  .positional(p)
                  .run(),
              vm);
    po::notify(vm);

    {
        /// Deal with command line errors or requests for help
        bool usage = false;

        if (vm.count("help")) {
            usage = true;
        } else if (vm.count("route") != 1) {
            cout << "Error: --route is a required argument\n" << endl;
            usage = true;
        }

        if (usage) {
            cout << "Usage: osvr_calibrate [config file name] [options]"
                 << endl;
            cout << desc << "\n";
            return 1;
        }
    }

    server = osvr::server::configureServerFromFile(configName);
    if (!server) {
        return -1;
    }

    cout << "Registering shutdown handler..." << endl;
    osvr::server::registerShutdownHandler<&handleShutdown>();

    std::string routes = server->getRoutes();
    cout << "Routes:\n" << routes << endl;

    cout << "Starting server mainloop..." << endl;
    server->start();

    cout << "Waiting a few seconds for the server to settle..." << endl;
    boost::this_thread::sleep(SETTLE_TIME);

    boost::this_thread::sleep(boost::posix_time::seconds(2));

    cout << "Stopping server mainloop..." << endl;
    server->stop();

    cout << "Server mainloop exited." << endl;

    return 0;
}