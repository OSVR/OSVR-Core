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
#include <ogvr/ClientKit/ContextC.h>
#include <ogvr/Client/ClientContext.h>
#include <ogvr/Client/CreateContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ClientContext ogvrClientInit(const char applicationIdentifier[],
                                  uint32_t /*flags*/) {
    return ::ogvr::client::createContext(applicationIdentifier);
}
OGVR_ReturnCode ogvrClientUpdate(OGVR_ClientContext ctx) {
    ctx->update();
    return OGVR_RETURN_SUCCESS;
}

OGVR_ReturnCode ogvrClientShutdown(OGVR_ClientContext ctx) {
    delete ctx;
    return OGVR_RETURN_SUCCESS;
}