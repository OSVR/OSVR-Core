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
#include <osvr/Common/RoutingKeys.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace routing_keys {
        const char *destination() { return "destination"; }

        const char *source() { return "source"; }

        const char *child() { return "child"; }
    } // namespace routing_keys
} // namespace common
} // namespace osvr
