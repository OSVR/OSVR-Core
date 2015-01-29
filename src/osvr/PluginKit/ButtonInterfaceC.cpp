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
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode
osvrDeviceButtonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_ButtonDeviceInterface *iface,
                          OSVR_IN OSVR_ButtonChanCount numChan) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", iface);
    opts->setButtons(numChan);
    return OSVR_RETURN_SUCCESS;
}