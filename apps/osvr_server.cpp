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
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

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
    std::string configName(osvr::server::getDefaultConfigFilename());
    if (argc > 1) {
        configName = argv[1];
    } else {
        out << "Using default config file - pass a filename on the command "
               "line to use a different one." << endl;
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