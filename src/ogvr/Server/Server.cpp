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
#include <ogvr/Server/Server.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace server {

    ServerPtr Server::createLocal() {
        // Not using make shared for encapsulation.
        ServerPtr ret(new Server());
        return ret;
    }

    Server::Server() {}

} // end of namespace server
} // end of namespace ogvr