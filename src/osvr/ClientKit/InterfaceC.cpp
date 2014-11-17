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
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetInterface(OSVR_ClientContext ctx,
                                       const char path[],
                                       OSVR_ClientInterface *iface) {
    /// @todo check for null ctx
    ::osvr::client::ClientInterfacePtr ret = ctx->getInterface(path);
    if (ret) {
        *iface = ret.get();
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientFreeInterface(OSVR_ClientInterface iface) {
    if (iface) {
        ::osvr::client::ClientContext &ctx = iface->getContext();
        /// This call returns a smart pointer - going to let it go out of scope
        /// here to delete.
        ctx.releaseInterface(iface);
    }
    return OSVR_RETURN_SUCCESS;
}