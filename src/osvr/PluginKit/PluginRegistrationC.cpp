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
#include <osvr/PluginKit/PluginRegistrationC.h>
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrPluginRegisterHardwareDetectCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_HardwareDetectCallback detectCallback,
    OSVR_IN_OPT void *userData) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrPluginRegisterHardwareDetectCallback",
                                    ctx);
    OSVR_DEV_VERBOSE("In osvrPluginRegisterHardwareDetectCallback with context "
                     << ctx);

    try {
        osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .registerHardwareDetectCallback(detectCallback, userData);
    } catch (std::exception &e) {
        std::cerr << "Error in osvrPluginRegisterHardwareDetectCallback - "
                     "caught exception reporting: " << e.what() << std::endl;
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrRegisterDriverInstantiationCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx, OSVR_IN_STRZ const char *name,
    OSVR_IN_PTR OSVR_DriverInstantiationCallback cb,
    OSVR_IN_OPT void *userData) {
    try {
        osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .registerDriverInstantiationCallback(name, cb, userData);
    } catch (std::exception &e) {
        std::cerr << "Error in osvrRegisterDriverInstantiationCallback - "
                     "caught exception reporting: " << e.what() << std::endl;
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrPluginRegisterDataWithDeleteCallback(
    OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
    OSVR_IN OSVR_PluginDataDeleteCallback deleteCallback,
    OSVR_INOUT_PTR void *pluginData) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrPluginRegisterDataWithDeleteCallback",
                                    ctx);
    osvr::pluginhost::PluginSpecificRegistrationContext *context =
        static_cast<osvr::pluginhost::PluginSpecificRegistrationContext *>(ctx);
    context->registerDataWithDeleteCallback(deleteCallback, pluginData);
    return OSVR_RETURN_SUCCESS;
}
