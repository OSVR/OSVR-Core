/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/ClientKit/ImagingC.h>
#include <osvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientFreeImage(OSVR_ClientContext ctx,
                                    OSVR_ImageBufferElement *buf) {
    auto ret = ctx->releaseObject(buf);
    return (ret ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE);
}