/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#include <boost/thread.hpp>

// Standard includes
#include <functional>
#include <string>

OSVR_DeviceInitOptions
osvrDeviceCreateInitOptions(OSVR_IN_PTR OSVR_PluginRegContext ctx) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT_CONSTRUCTOR("osvrDeviceCreateInitOptions",
                                                ctx);
    return new OSVR_DeviceInitObject(ctx);
}

OSVR_ReturnCode osvrDeviceSendData(OSVR_IN_PTR OSVR_DeviceToken dev,
                                   OSVR_IN_PTR OSVR_MessageType msg,
                                   OSVR_IN_READS(len) const char *bytestream,
                                   OSVR_IN size_t len) {
    OSVR_DEV_VERBOSE(
        "In osvrDeviceSendData, trying to send a message of length " << len);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData device token", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData message type", msg);
    dev->sendData(msg, bytestream, len);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceSendTimestampedData(
    OSVR_IN_PTR OSVR_DeviceToken dev, OSVR_IN_PTR OSVR_TimeValue *timestamp,
    OSVR_IN_PTR OSVR_MessageType msg, OSVR_IN_READS(len) const char *bytestream,
    OSVR_IN size_t len) {
    OSVR_DEV_VERBOSE(
        "In osvrDeviceSendData, trying to send a timestamped message of length "
        << len);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData device token", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendData message type", msg);
    dev->sendData(*timestamp, msg, bytestream, len);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceSendJsonDescriptor(OSVR_IN_PTR OSVR_DeviceToken dev,
                                             OSVR_IN_READS(len)
                                                 const char *json,
                                             OSVR_IN size_t len) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendJsonDescriptor", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSendJsonDescriptor descriptor",
                                    json);

    dev->setDeviceDescriptor(std::string(json, len));
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceRegisterMessageType(OSVR_IN_PTR OSVR_PluginRegContext ctx,
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
osvrDeviceGenericInit(OSVR_DeviceInitOptions options, const char *name,
                      OSVR_DeviceToken *device, FactoryFunction f) {
    options->setName(name);

    return osvrDeviceGenericInit(options, device, f);
}

template <typename FactoryFunction>
inline static OSVR_ReturnCode
osvrDeviceGenericInit(OSVR_PluginRegContext ctx, const char *name,
                      OSVR_DeviceToken *device, FactoryFunction f) {
    OSVR_DeviceInitOptions options = osvrDeviceCreateInitOptions(ctx);

    return osvrDeviceGenericInit(options, name, device, f);
}

OSVR_ReturnCode osvrDeviceSyncInit(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                                   OSVR_IN_STRZ const char *name,
                                   OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSyncInit", ctx);
    OSVR_DEV_VERBOSE("In osvrDeviceSyncInit for a device named " << name);
    return osvrDeviceGenericInit(ctx, name, device,
                                 OSVR_DeviceTokenObject::createSyncDevice);
}
OSVR_ReturnCode
osvrDeviceSyncInitWithOptions(OSVR_IN_PTR OSVR_PluginRegContext,
                              OSVR_IN_STRZ const char *name,
                              OSVR_IN_PTR OSVR_DeviceInitOptions options,
                              OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSyncInitWithOptions", options);
    return osvrDeviceGenericInit(options, name, device,
                                 OSVR_DeviceTokenObject::createSyncDevice);
}

OSVR_ReturnCode
osvrDeviceRegisterUpdateCallback(OSVR_IN_PTR OSVR_DeviceToken dev,
                                 OSVR_IN OSVR_DeviceUpdateCallback
                                     updateCallback,
                                 OSVR_IN_OPT void *userData) {
    OSVR_DEV_VERBOSE("In osvrDeviceRegisterUpdateCallback");
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT(
        "osvrDeviceRegisterUpdateCallback device token", dev);
    dev->setUpdateCallback(std::bind(updateCallback, userData));
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceAsyncInit(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                                    OSVR_IN_STRZ const char *name,
                                    OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAsyncInit", ctx);
    OSVR_DEV_VERBOSE("In osvrDeviceAsyncInit for a device named " << name);
    return osvrDeviceGenericInit(ctx, name, device,
                                 OSVR_DeviceTokenObject::createAsyncDevice);
}

OSVR_ReturnCode
osvrDeviceAsyncInitWithOptions(OSVR_IN_PTR OSVR_PluginRegContext,
                               OSVR_IN_STRZ const char *name,
                               OSVR_IN_PTR OSVR_DeviceInitOptions options,
                               OSVR_OUT_PTR OSVR_DeviceToken *device) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAsyncInitWithOptions", options);
    return osvrDeviceGenericInit(options, name, device,
                                 OSVR_DeviceTokenObject::createAsyncDevice);
}

OSVR_ReturnCode osvrDeviceMicrosleep(OSVR_IN uint64_t microseconds) {
    boost::this_thread::sleep(boost::posix_time::microseconds(microseconds));
    return OSVR_RETURN_SUCCESS;
}
