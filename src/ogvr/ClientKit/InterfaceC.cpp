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
#include <ogvr/ClientKit/InterfaceC.h>
#include <ogvr/Client/ClientInterface.h>
#include <ogvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ReturnCode ogvrClientGetInterface(OGVR_ClientContext ctx,
                                       const char path[],
                                       OGVR_ClientInterface *iface) {
    /// @todo check for null ctx
    ::ogvr::client::ClientInterfacePtr ret = ctx->getInterface(path);
    if (ret) {
        *iface = ret.get();
        return OGVR_RETURN_SUCCESS;
    }
    return OGVR_RETURN_FAILURE;
}

OGVR_ReturnCode ogvrClientFreeInterface(OGVR_ClientInterface iface) {
    if (iface) {
        ::ogvr::client::ClientContext &ctx = iface->getContext();
        /// This call returns a smart pointer - going to let it go out of scope
        /// here to delete.
        ctx.releaseInterface(iface);
    }
    return OGVR_RETURN_SUCCESS;
}