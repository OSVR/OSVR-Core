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
#include <ogvr/Util/Verbosity.h>
#include "PluginSpecificRegistrationContext.h"

// Library/third-party includes
// - none

// Standard includes
#include <assert.h>

OGVR_PluginReturnCode
ogvrPluginRegisterHardwarePollCallback(OGVR_PluginRegContext ctx,
                                       OGVRHardwarePollCallback pollCallback,
                                       void *userData) {
    assert(ctx);
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrPluginRegisterHardwarePollCallback");

    ogvr::PluginSpecificRegistrationContext *context =
        static_cast<ogvr::PluginSpecificRegistrationContext *>(ctx);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrPluginRegisterDataWithDeleteCallback(
    OGVR_PluginRegContext ctx, OGVR_PluginDataDeleteCallback deleteCallback,
    void *pluginData) {
    assert(ctx);
    ogvr::PluginSpecificRegistrationContext *context =
        static_cast<ogvr::PluginSpecificRegistrationContext *>(ctx);
    context->registerDataWithDeleteCallback(deleteCallback, pluginData);
    return OGVR_PLUGIN_SUCCESS;
}
