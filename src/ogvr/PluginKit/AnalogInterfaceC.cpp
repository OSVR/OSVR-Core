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
#include <ogvr/PluginKit/AnalogInterfaceC.h>
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ReturnCode
ogvrDeviceAnalogConfigure(OGVR_INOUT_PTR OGVR_DeviceInitOptions opts,
                          OGVR_IN OGVR_AnalogChanCount numChan) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceAnalogConfigure", opts);
    if (numChan == 0) {
    }
}