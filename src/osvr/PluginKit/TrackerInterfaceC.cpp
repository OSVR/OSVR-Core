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
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode
osvrDeviceTrackerConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_TrackerDeviceInterface *iface) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceTrackerConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceTrackerConfigure", iface);
    opts->setTracker();
    return OSVR_RETURN_SUCCESS;
}