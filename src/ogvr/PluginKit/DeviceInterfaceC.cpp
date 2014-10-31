/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/DeviceInterfaceC.h>
#include <ogvr/PluginKit/DeviceToken.h>
#include <ogvr/PluginKit/Connection.h>
#include <ogvr/Util/Verbosity.h>
#include "HandleNullContext.h"
#include "PluginSpecificRegistrationContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_PluginReturnCode ogvrDeviceSendData(OGVR_DeviceToken dev,
                                         const char *bytestream, size_t len) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE(
        "In ogvrDeviceSendData, trying to send a message of length " << len);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceSyncInit(OGVR_PluginRegContext ctx,
                                         const char *name,
                                         OGVR_DeviceToken *device) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncInit for a device named " << name);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceSyncRegisterUpdateCallback(
    OGVR_DeviceToken device, OGVR_SyncDeviceUpdateCallback updateCallback,
    void *userData) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncRegisterUpdateCallback");
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceAsyncInit(OGVR_PluginRegContext ctx,
                                          const char *name,
                                          OGVR_DeviceToken *device) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceAsyncInit", ctx);
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncInit for a device named " << name);

    ogvr::PluginSpecificRegistrationContext *context =
        static_cast<ogvr::PluginSpecificRegistrationContext *>(ctx);
    // Compute the name by combining plugin name with the given name
    std::string qualifiedName = context->getName() + "/" + name;

    OGVR_DEV_VERBOSE("Qualified name: " << qualifiedName);

    ogvr::RegistrationContext &overallContext(context->getParent());
    // Extract the connection from the overall context
    ogvr::ConnectionPtr conn =
        ogvr::Connection::retrieveConnection(overallContext);
    if (!conn) {
        OGVR_DEV_VERBOSE("ogvrDeviceAsyncInit Got a null Connection pointer - "
                         "this shouldn't happen!");
        return OGVR_PLUGIN_FAILURE;
    }
    ogvr::DeviceTokenPtr dev =
        ogvr::DeviceToken::createAsyncDevice(qualifiedName, conn);
    if (!dev) {
        OGVR_DEV_VERBOSE("DeviceToken::createAsyncDevice returned a null "
                         "pointer - this shouldn't happen!");
        return OGVR_PLUGIN_FAILURE;
    }
    *device = dev.release();
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode
ogvrDeviceAsyncStartWaitLoop(OGVR_DeviceToken device,
                             OGVR_AsyncDeviceWaitCallback waitCallback,
                             void *userData) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncStartWaitLoop");
    return OGVR_PLUGIN_SUCCESS;
}