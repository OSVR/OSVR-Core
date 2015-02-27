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
#include <osvr/Common/CreateDevice.h>
#include "DeviceWrapper.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    BaseDevicePtr createClientDevice(std::string const &name,
                                     vrpn_ConnectionPtr const &conn) {
        auto ret = make_shared<DeviceWrapper>(name, conn, true);
        return ret;
    }
    BaseDevicePtr createServerDevice(std::string const &name,
                                     vrpn_ConnectionPtr const &conn) {
        auto ret = make_shared<DeviceWrapper>(name, conn, false);
        return ret;
    }
} // namespace common
} // namespace osvr