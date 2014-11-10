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
#include <ogvr/PluginKit/PluginRegistration.h>
#include <ogvr/PluginHost/RegistrationContext.h>
#include <ogvr/Connection/DeviceToken.h>
#include <ogvr/Connection/MessageType.h>
#include <ogvr/Connection/Connection.h>
#include <ogvr/Util/Verbosity.h>
#include "HandleNullContext.h"
#include "DeviceInitObject.h"

// Library/third-party includes
// - none

// Standard includes
#include <functional>

OGVR_DeviceInitOptions
ogvrDeviceCreateInitOptions(OGVR_INOUT_PTR OGVR_PluginRegContext ctx) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT_CONSTRUCTOR("ogvrDeviceCreateInitOptions",
                                                ctx);
    return new OGVR_DeviceInitObject(ctx);
}

OGVR_ReturnCode ogvrDeviceSendData(OGVR_INOUT_PTR OGVR_DeviceToken dev,
                                   OGVR_IN_PTR OGVR_MessageType msg,
                                   OGVR_IN_READS(len) const char *bytestream,
                                   OGVR_IN size_t len) {
    OGVR_DEV_VERBOSE(
        "In ogvrDeviceSendData, trying to send a message of length " << len);
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceSendData device token", dev);
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceSendData message type", msg);
    ogvr::connection::DeviceToken *device =
        static_cast<ogvr::connection::DeviceToken *>(dev);
    ogvr::connection::MessageType *msgType =
        static_cast<ogvr::connection::MessageType *>(msg);
    device->sendData(msgType, bytestream, len);
    return OGVR_RETURN_SUCCESS;
}

OGVR_ReturnCode
ogvrDeviceSendTimestampedData(OGVR_INOUT_PTR OGVR_DeviceToken dev,
                              OGVR_IN_PTR struct OGVR_TimeValue *timestamp,
                              OGVR_IN_PTR OGVR_MessageType msg,
                              OGVR_IN_READS(len) const char *bytestream,
                              OGVR_IN size_t len) {
    OGVR_DEV_VERBOSE(
        "In ogvrDeviceSendData, trying to send a timestamped message of length "
        << len);
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceSendData device token", dev);
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceSendData message type", msg);
    ogvr::connection::DeviceToken *device =
        static_cast<ogvr::connection::DeviceToken *>(dev);
    ogvr::connection::MessageType *msgType =
        static_cast<ogvr::connection::MessageType *>(msg);
    device->sendData(*timestamp, msgType, bytestream, len);
    return OGVR_RETURN_SUCCESS;
}

OGVR_ReturnCode
ogvrDeviceRegisterMessageType(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                              OGVR_IN_STRZ const char *name,
                              OGVR_OUT_PTR OGVR_MessageType *msgtype) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceRegisterMessageType", ctx);
    OGVR_DEV_VERBOSE("In ogvrDeviceRegisterMessageType for a message named "
                     << name);

    // Extract the connection from the overall context
    ogvr::connection::ConnectionPtr conn =
        ogvr::connection::Connection::retrieveConnection(
            ogvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
                .getParent());
    ogvr::connection::MessageTypePtr ret = conn->registerMessageType(name);

    // Transfer ownership of the message type object to the plugin context.
    try {
        *msgtype =
            ogvr::pluginkit::registerObjectForDeletion(ctx, ret.release());
    } catch (std::exception &e) {
        std::cerr << "Error in ogvrDeviceRegisterMessageType: " << e.what()
                  << std::endl;
        return OGVR_RETURN_FAILURE;
    } catch (...) {
        return OGVR_RETURN_FAILURE;
    }
    return OGVR_RETURN_SUCCESS;
}

template <typename FactoryFunction>
inline static OGVR_ReturnCode
ogvrDeviceGenericInit(OGVR_PluginRegContext ctx, const char *name,
                      OGVR_DeviceToken *device, FactoryFunction f) {
    // Compute the name by combining plugin name with the given name
    std::string qualifiedName =
        ogvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .getName() +
        "/" + name;

    OGVR_DEV_VERBOSE("Qualified name: " << qualifiedName);

    // Extract the connection from the overall context
    ogvr::connection::ConnectionPtr conn =
        ogvr::connection::Connection::retrieveConnection(
            ogvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
                .getParent());
    if (!conn) {
        OGVR_DEV_VERBOSE(
            "ogvrDeviceGenericInit Got a null Connection pointer - "
            "this shouldn't happen!");
        return OGVR_RETURN_FAILURE;
    }
    ogvr::connection::DeviceTokenPtr dev = f(qualifiedName, conn);
    if (!dev) {
        OGVR_DEV_VERBOSE("Device token factory returned a null "
                         "pointer - this shouldn't happen!");
        return OGVR_RETURN_FAILURE;
    }
    // Transfer ownership of the device token object to the plugin context.
    try {
        *device =
            ogvr::pluginkit::registerObjectForDeletion(ctx, dev.release());
    } catch (std::exception &e) {
        std::cerr << "Error in ogvrDeviceGenericInit: " << e.what()
                  << std::endl;
        return OGVR_RETURN_FAILURE;
    } catch (...) {
        return OGVR_RETURN_FAILURE;
    }
    return OGVR_RETURN_SUCCESS;
}

OGVR_ReturnCode ogvrDeviceSyncInit(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                                   OGVR_IN_STRZ const char *name,
                                   OGVR_OUT_PTR OGVR_DeviceToken *device) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceSyncInit", ctx);
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncInit for a device named " << name);
    return ogvrDeviceGenericInit(
        ctx, name, device, ogvr::connection::DeviceToken::createSyncDevice);
}

OGVR_ReturnCode
ogvrDeviceSyncRegisterUpdateCallback(OGVR_INOUT_PTR OGVR_DeviceToken device,
                                     OGVR_IN OGVR_SyncDeviceUpdateCallback
                                         updateCallback,
                                     OGVR_IN_OPT void *userData) {
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncRegisterUpdateCallback");
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT(
        "ogvrDeviceSyncRegisterUpdateCallback device token", device);
    ogvr::connection::DeviceToken *dev =
        static_cast<ogvr::connection::DeviceToken *>(device);
    try {
        dev->setSyncUpdateCallback(std::bind(updateCallback, userData));
    } catch (std::logic_error &e) {
        OGVR_DEV_VERBOSE("Caught a logic error setting update callback - "
                         "likely that this isn't a synchronous device token. "
                         "Details: "
                         << e.what());
        return OGVR_RETURN_FAILURE;
    }
    return OGVR_RETURN_SUCCESS;
}

OGVR_ReturnCode ogvrDeviceAsyncInit(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                                    OGVR_IN_STRZ const char *name,
                                    OGVR_OUT_PTR OGVR_DeviceToken *device) {
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceAsyncInit", ctx);
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncInit for a device named " << name);
    return ogvrDeviceGenericInit(
        ctx, name, device, ogvr::connection::DeviceToken::createAsyncDevice);
}

OGVR_ReturnCode
ogvrDeviceAsyncStartWaitLoop(OGVR_INOUT_PTR OGVR_DeviceToken device,
                             OGVR_IN OGVR_AsyncDeviceWaitCallback waitCallback,
                             OGVR_IN_OPT void *userData) {
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncStartWaitLoop");
    OGVR_PLUGIN_HANDLE_NULL_CONTEXT("ogvrDeviceAsyncStartWaitLoop device token",
                                    device);
    ogvr::connection::DeviceToken *dev =
        static_cast<ogvr::connection::DeviceToken *>(device);
    try {
        dev->setAsyncWaitCallback(std::bind(waitCallback, userData));
    } catch (std::logic_error &e) {
        OGVR_DEV_VERBOSE("Caught a logic error setting update callback - "
                         "likely that this isn't an asynchronous device token. "
                         "Details: "
                         << e.what());
        return OGVR_RETURN_FAILURE;
    }
    return OGVR_RETURN_SUCCESS;
}