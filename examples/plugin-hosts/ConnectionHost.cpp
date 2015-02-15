/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <exception>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Must supply a plugin name to load." << std::endl;
        return 1;
    }
    osvr::pluginhost::RegistrationContext ctx;
    osvr::connection::ConnectionPtr conn =
        osvr::connection::Connection::createLocalConnection();
    osvr::connection::Connection::storeConnection(ctx, conn);

    try {
        std::cout << "Trying to load plugin " << argv[1] << std::endl;
        ctx.loadPlugin(argv[1]);
        std::cout << "Successfully loaded plugin, control returned to host "
                     "application!" << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Caught exception tring to load " << argv[1] << ": "
                  << e.what() << std::endl;
        return 1;
    }
    int count = 20;
    std::cout << "Running connection processing " << count << " times"
              << std::endl;
    for (int i = 0; i < count; ++i) {
        conn->process();
    }

    return 0;
}