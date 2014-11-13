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
#include <ogvr/Client/CreateContext.h>
#include "VRPNContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace client {

    ClientContext *createContext(const char appId[], const char host[]) {
        if (!appId || strlen(appId) == 0) {
            return NULL;
        }
        VRPNContext *ret = new VRPNContext(appId, host);
        return ret;
    }

} // namespace client
} // namespace ogvr
