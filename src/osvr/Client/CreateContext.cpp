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
#include <osvr/Client/CreateContext.h>
#include "VRPNContext.h"

// Library/third-party includes
// - none

// Standard includes
#include <cstring>

namespace osvr {
namespace client {

    ClientContext *createContext(const char appId[], const char host[]) {
        if (!appId || std::strlen(appId) == 0) {
            return nullptr;
        }
        VRPNContext *ret = new VRPNContext(appId, host);
        return ret;
    }

} // namespace client
} // namespace osvr
