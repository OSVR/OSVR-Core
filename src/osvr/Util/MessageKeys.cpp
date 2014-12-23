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
#include <osvr/Util/MessageKeys.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    namespace messagekeys {
        const char *systemSender() { return "OSVR"; }
        const char *routingData() {
            return "org.opengoggles.system.routingdata";
        }
    } // namespace messagekeys
} // namespace util
} // namespace osvr