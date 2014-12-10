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
#include <boost/assert.hpp>

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetInterface(OSVR_ClientContext ctx,
                                       const char path[],
                                       OSVR_ClientInterface *iface) {
    if (nullptr == ctx) {
        return OSVR_RETURN_FAILURE;
    }
    ::osvr::client::ClientInterfacePtr ret = ctx->getInterface(path);
    if (ret) {
        *iface = ret.get();
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientFreeInterface(OSVR_ClientInterface iface) {
    if (nullptr == iface) {
        /// Return failure if given a null interface
        return OSVR_RETURN_FAILURE;
    }
    ::osvr::client::ClientContext &ctx = iface->getContext();
    /// This call returns a smart pointer - going to let it go out of scope
    /// here to delete.
    ::osvr::client::ClientInterfacePtr ptr(ctx.releaseInterface(iface));
    if (!ptr) {
        /// Return failure if the context didn't have a record of this
        /// interface.
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}