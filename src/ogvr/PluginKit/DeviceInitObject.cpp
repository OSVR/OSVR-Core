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
#include "DeviceInitObject.h"
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_DeviceInitObject::OGVR_DeviceInitObject(OGVR_PluginRegContext ctx)
    : context(ogvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)) {}

void OGVR_DeviceInitObject::setAnalogs(OGVR_AnalogChanCount numAnalogs) {
    if (numAnalogs == 0) {
        analogs.reset();
    } else {
        analogs = numAnalogs;
    }
}