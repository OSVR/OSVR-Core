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
#include "BinaryLocation.h"

// Library/third-party includes
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Standard includes
// - none

namespace osvr {
namespace pluginhost {

#ifdef _WIN32
    std::string getBinaryLocation() {
        char buf[512] = {0};
        DWORD len = GetModuleFileName(NULL, buf, sizeof(buf));
        std::string ret;
        if (0 != len) {
            ret.assign(buf, len);
        }
        return ret;
    }
#else
#error "Not yet implemented for this platform!"
    std::string getBinaryLocation() {
        std::string ret;
        return ret;
    }
#endif
} // namespace pluginhost
} // namespace osvr
