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
#include <ogvr/Client/ContextC.h>
#include "ContextImpl.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ClientContext ogvrClientInit(uint32_t /*flags*/) {
    return new OGVR_ClientContextObject();
}

OGVR_ReturnCode ogvrClientShutdown(OGVR_ClientContext ctx) {
    delete ctx;
    return OGVR_RETURN_SUCCESS;
}