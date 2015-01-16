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
#include <osvr/ClientKit/ContextC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/CreateContext.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdlib.h>

static const char HOST_ENV_VAR[] = "OSVR_HOST";

OSVR_ClientContext osvrClientInit(const char applicationIdentifier[],
                                  uint32_t /*flags*/) {
    char *host = ::getenv(HOST_ENV_VAR);
    if (nullptr != host) {
        OSVR_DEV_VERBOSE("Connecting to non-default host " << host);
        return ::osvr::client::createContext(applicationIdentifier, host);
    } else {
        OSVR_DEV_VERBOSE("Connecting to default (local) host");
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