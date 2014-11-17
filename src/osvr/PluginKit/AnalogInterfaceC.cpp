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
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include "DeviceInitObject.h"
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode
osvrDeviceAnalogConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_IN OSVR_AnalogChanCount numChan) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogConfigure", opts);
    opts->setAnalogs(numChan);
    return OSVR_RETURN_SUCCESS;
}