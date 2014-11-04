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
#include <ogvr/PluginKit/PluginRegistrationC.h>
#include "HandleNullContext.h"
#include <ogvr/Util/Verbosity.h>
#include "PluginSpecificRegistrationContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_PluginReturnCode ogvrPluginRegisterHardwarePollCallback(
    OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
    OGVR_IN OGVRHardwarePollCallback pollCallback, OGVR_IN_OPT void *userData) {

    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrPluginRegisterHardwarePollCallback",
                                    ctx);
    OGVR_DEV_VERBOSE("In ogvrPluginRegisterHardwarePollCallback with context "
                     << ctx);

    ogvr::PluginSpecificRegistrationContext *context =
        static_cast<ogvr::PluginSpecificRegistrationContext *>(ctx);
    context->registerHardwarePollCallback(pollCallback, userData);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrPluginRegisterDataWithDeleteCallback(
    OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
    OGVR_IN OGVR_PluginDataDeleteCallback deleteCallback,
    OGVR_INOUT_PTR void *pluginData) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrPluginRegisterDataWithDeleteCallback",
                                    ctx);
    ogvr::PluginSpecificRegistrationContext *context =
        static_cast<ogvr::PluginSpecificRegistrationContext *>(ctx);
    context->registerDataWithDeleteCallback(deleteCallback, pluginData);
    return OGVR_PLUGIN_SUCCESS;
}
