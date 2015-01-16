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
#include "DeviceInitObject.h"
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_DeviceInitObject::OSVR_DeviceInitObject(OSVR_PluginRegContext ctx)
    : context(osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)) {}

void OSVR_DeviceInitObject::setAnalogs(OSVR_AnalogChanCount numAnalogs) {
    if (numAnalogs == 0) {
        analogs.reset();
    } else {
        analogs = numAnalogs;
    }
}