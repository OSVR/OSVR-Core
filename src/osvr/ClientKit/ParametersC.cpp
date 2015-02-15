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
#include <osvr/ClientKit/ParametersC.h>
#include <osvr/Client/ClientContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetStringParameterLength(OSVR_ClientContext ctx,
                                                   const char path[],
                                                   size_t *len) {
    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (len == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    std::string val = ctx->getStringParameter(path);
    *len = val.empty() ? 0 : (val.size() + 1);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetStringParameter(OSVR_ClientContext ctx,
                                             const char path[], char *buf,
                                             size_t len) {
    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (buf == nullptr) {
        return OSVR_RETURN_FAILURE;
    }

    std::string val = ctx->getStringParameter(path);
    if (val.size() + 1 > len) {
        /// buffer too small.
        return OSVR_RETURN_FAILURE;
    }
    val.copy(buf, val.size());
    buf[val.size()] = '\0';
    return OSVR_RETURN_SUCCESS;
}