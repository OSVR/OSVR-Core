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
#include <ogvr/PluginKit/RegistrationContext.h>
#include <ogvr/PluginKit/Connection.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <exception>


int main(int argc, char * argv[]) {
    if (argc < 2) {
        std::cerr << "Must supply a plugin name to load." << std::endl;
        return 1;
    }
	ogvr::RegistrationContext ctx;
	ogvr::ConnectionPtr conn = ogvr::Connection::createLocalConnection();
	ogvr::Connection::storeConnection(ctx, conn);

    try {
        std::cout << "Trying to load plugin " << argv[1] << std::endl;
		ctx.loadPlugin(argv[1]);
        std::cout << "Successfully loaded plugin, control returned to host application!" << std::endl;
        return 0;
    } catch (std::exception & e) {
        std::cerr << "Caught exception tring to load " << argv[1] << ": " << e.what() << std::endl;
        return 1;
    }
    std::cerr << "Failed in a weird way - not a std::exception." << std::endl;
    return 2;
}