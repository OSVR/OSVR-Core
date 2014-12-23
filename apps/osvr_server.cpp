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
#include <osvr/Server/Server.h>
#include <osvr/Server/ConfigureServer.h>
#include <osvr/Server/RegisterShutdownHandler.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using std::cout;
using std::cerr;
using std::endl;

static osvr::server::ServerPtr server;

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    cout << "Received shutdown signal..." << endl;
    server->signalStop();
}

int main(int argc, char *argv[]) {
    std::string configName = "osvr_server_config.json";
    if (argc > 1) {
        configName = argv[1];
    } else {
        cout << "Using default config file - pass a filename on the command "
                "line to use a different one." << endl;
    }

    cout << "Using config file '" << configName << "'" << endl;

    {
        std::ifstream config(configName);
        if (!config.good()) {
            cerr << "\n"
                 << "Could not open config file!" << endl;
            cerr << "Searched in the current directory; file may be "
                    "misspelled, missing, or in a different directory." << endl;
            return 1;
        }

        osvr::server::ConfigureServer srvConfig;
        cout << "Constructing server as configured..." << endl;
        try {
            srvConfig.loadConfig(config);
            server = srvConfig.constructServer();
        } catch (std::exception &e) {
            cerr << "Caught exception constructing server from JSON config "
                    "file: " << e.what() << endl;
            return 1;
        }

        {
            cout << "Loading plugins..." << endl;
            bool success = srvConfig.loadPlugins();
            if (!srvConfig.getSuccessfulPlugins().empty()) {
                cout << "Successful plugins:" << endl;
                for (auto const &plugin : srvConfig.getSuccessfulPlugins()) {
                    cout << " - " << plugin << endl;
                }
                cout << "\n";
            }
            if (!srvConfig.getFailedPlugins().empty()) {
                cout << "Failed plugins:" << endl;
                for (auto const &pluginError : srvConfig.getFailedPlugins()) {
                    cout << " - " << pluginError.first << "\t"
                         << pluginError.second << endl;
                }
                cout << "\n";
            }

            cout << "\n";
        }

        {
            cout << "Instantiating configured drivers..." << endl;
            bool success = srvConfig.instantiateDrivers();
            if (!srvConfig.getSuccessfulInstantiations().empty()) {
                cout << "Successes:" << endl;
                for (auto const &driver :
                     srvConfig.getSuccessfulInstantiations()) {
                    cout << " - " << driver << endl;
                }
                cout << "\n";
            }
            if (!srvConfig.getFailedInstantiations().empty()) {
                cout << "Errors:" << endl;
                for (auto const &error : srvConfig.getFailedInstantiations()) {
                    cout << " - " << error.first << "\t" << error.second
                         << endl;
                }
                cout << "\n";
            }
            cout << "\n";
        }

        srvConfig.processRoutes();

    } // end of scope for server config object

    cout << "Registering shutdown handler..." << endl;
    osvr::server::registerShutdownHandler<&handleShutdown>();

    cout << "Triggering a hardware detection..." << endl;
    server->triggerHardwareDetect();

    cout << "Starting server mainloop..." << endl;
    server->startAndAwaitShutdown();

    cout << "Server mainloop exited." << endl;

    return 0;
}