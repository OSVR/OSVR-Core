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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/PluginKit/PluginRegistration.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Connection/MessageType.h>
#include <osvr/Connection/Connection.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Util/Verbosity.h>
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
#include <functional>

OSVR_DeviceInitOptions
osvrDeviceCreateInitOptions(OSVR_INOUT_PTR OSVR_PluginRegContext ctx) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT_CONSTRUCTOR("osvrDeviceCreateInitOptions",
                                                ctx);
    return new OSVR_DeviceInitObject(ctx);
}

OSVR_ReturnCode osvrDeviceSendData(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                   OSVR_IN_PTR OSVR_MessageType msg,
                                   OSVR_IN_READS(len) const char *bytestream,
                                   OSVR_IN size_t len) {
    OSVR_DEV_VERBOSE(
        "In osvrDeviceSendData, trying to send a message of length " << len);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData device token", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData message type", msg);
    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);
    osvr::connection::MessageType *msgType =
        static_cast<osvr::connection::MessageType *>(msg);
    device->sendData(msgType, bytestream, len);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceSendTimestampedData(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR struct OSVR_TimeValue *timestamp,
                              OSVR_IN_PTR OSVR_MessageType msg,
                              OSVR_IN_READS(len) const char *bytestream,
                              OSVR_IN size_t len) {
    OSVR_DEV_VERBOSE(
        "In osvrDeviceSendData, trying to send a timestamped message of length "
        << len);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData device token", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData message type", msg);
    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);
    osvr::connection::MessageType *msgType =
        static_cast<osvr::connection::MessageType *>(msg);
    device->sendData(*timestamp, msgType, bytestream, len);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceRegisterMessageType(OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
                              OSVR_IN_STRZ const char *name,
                              OSVR_OUT_PTR OSVR_MessageType *msgtype) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceRegisterMessageType", ctx);
    OSVR_DEV_VERBOSE("In osvrDeviceRegisterMessageType for a message named "
                     << name);

    // Extract the connection from the overall context
    osvr::connection::ConnectionPtr conn =
        osvr::connection::Connection::retrieveConnection(
            osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
                .getParent());
    osvr::connection::MessageTypePtr ret = conn->registerMessageType(name);

    // Transfer ownership of the message type object to the plugin context.
    try {
        *msgtype =
            osvr::pluginkit::registerObjectForDeletion(ctx, ret.release());
    } catch (std::exception &e) {
        std::cerr << "Error in osvrDeviceRegisterMessageType: " << e.what()
                  << std::endl;
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

template <typename FactoryFunction>
inline static OSVR_ReturnCode
osvrDeviceGenericInit(OSVR_DeviceInitOptions options, OSVR_DeviceToken *device,
                      FactoryFunction f) {

    osvr::connection::DeviceTokenPtr dev = f(*options);
    if (!dev) {
        OSVR_DEV_VERBOSE("Device token factory returned a null "
                         "pointer - this shouldn't happen!");
        return OSVR_RETURN_FAILURE;
    }
    // Transfer ownership of the device token object to the plugin context.
    try {
        *device =
            options->getContext()->registerDataWithGenericDelete(dev.release());
            /// @todo Is this too late to delete? Can we delete it earlier?
            options->getContext()->registerDataWithGenericDelete(options);
    } catch (std::exception &e) {
        std::cerr << "Error in osvrDeviceGenericInit: " << e.what()
                  << std::endl;
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

template <typename FactoryFunction>
inline static OSVR_ReturnCode
osvrDeviceGenericInit(OSVR_PluginRegContext ctx, const char *name,
                      OSVR_DeviceToken *device, FactoryFunction f) {
    OSVR_DeviceInitOptions options = osvrDeviceCreateInitOptions(ctx);
    options->setName(name);

    return osvrDeviceGenericInit(options, device, f);
}

OSVR_ReturnCode osvrDeviceSyncInit(OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
                                   OSVR_IN_STRZ const char *name,
                                   OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSyncInit", ctx);
    OSVR_DEV_VERBOSE("In osvrDeviceSyncInit for a device named " << name);
    return osvrDeviceGenericInit(
        ctx, name, device, osvr::connection::DeviceToken::createSyncDevice);
}
OSVR_ReturnCode
osvrDeviceSyncInitWithOptions(OSVR_INOUT_PTR OSVR_PluginRegContext,
OSVR_IN_STRZ const char *name,
OSVR_INOUT_PTR OSVR_DeviceInitOptions options,
OSVR_OUT_PTR OSVR_DeviceToken *device) {
    options->setName(name);
    return osvrDeviceGenericInit(options, device, osvr::connection::DeviceToken::createSyncDevice);
}

OSVR_ReturnCode
osvrDeviceSyncRegisterUpdateCallback(OSVR_INOUT_PTR OSVR_DeviceToken device,
                                     OSVR_IN OSVR_SyncDeviceUpdateCallback
                                         updateCallback,
                                     OSVR_IN_OPT void *userData) {
    OSVR_DEV_VERBOSE("In osvrDeviceSyncRegisterUpdateCallback");
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT(
        "osvrDeviceSyncRegisterUpdateCallback device token", device);
    osvr::connection::DeviceToken *dev =
        static_cast<osvr::connection::DeviceToken *>(device);
    try {
        dev->setSyncUpdateCallback(std::bind(updateCallback, userData));
    } catch (std::logic_error &e) {
        OSVR_DEV_VERBOSE("Caught a logic error setting update callback - "
                         "likely that this isn't a synchronous device token. "
                         "Details: "
                         << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceAsyncInit(OSVR_INOUT_PTR OSVR_PluginRegContext ctx,
                                    OSVR_IN_STRZ const char *name,
                                    OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAsyncInit", ctx);
    OSVR_DEV_VERBOSE("In osvrDeviceAsyncInit for a device named " << name);
    return osvrDeviceGenericInit(
        ctx, name, device, osvr::connection::DeviceToken::createAsyncDevice);
}

OSVR_ReturnCode
osvrDeviceAsyncStartWaitLoop(OSVR_INOUT_PTR OSVR_DeviceToken device,
                             OSVR_IN OSVR_AsyncDeviceWaitCallback waitCallback,
                             OSVR_IN_OPT void *userData) {
    OSVR_DEV_VERBOSE("In osvrDeviceAsyncStartWaitLoop");
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAsyncStartWaitLoop device token",
                                    device);
    osvr::connection::DeviceToken *dev =
        static_cast<osvr::connection::DeviceToken *>(device);
    try {
        dev->setAsyncWaitCallback(std::bind(waitCallback, userData));
    } catch (std::logic_error &e) {
        OSVR_DEV_VERBOSE("Caught a logic error setting update callback - "
                         "likely that this isn't an asynchronous device token. "
                         "Details: "
                         << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}