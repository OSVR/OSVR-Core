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
#include <osvr/ClientKit/ContextC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/CreateContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

static const char HOST_ENV_VAR[] = "OSVR_HOST";

OSVR_ClientContext osvrClientInit(const char applicationIdentifier[],
                                  uint32_t /*flags*/) {
    char *host = ::getenv(HOST_ENV_VAR);
    if (nullptr != host) {
        return ::osvr::client::createContext(applicationIdentifier, host);
    } else {
        return ::osvr::client::createContext(applicationIdentifier);
    }
}
OSVR_ReturnCode osvrClientUpdate(OSVR_ClientContext ctx) {
    ctx->update();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientShutdown(OSVR_ClientContext ctx) {
    delete ctx;
    return OSVR_RETURN_SUCCESS;
}