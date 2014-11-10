/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_DeviceInterfaceC_h_GUID_8B82B108_1B20_4D80_9FE5_C31E424E3652
#define INCLUDED_DeviceInterfaceC_h_GUID_8B82B108_1B20_4D80_9FE5_C31E424E3652

/* Internal Includes */
#include <ogvr/PluginKit/Export.h>
#include <ogvr/PluginKit/CommonC.h>
#include <ogvr/Util/DeviceCallbackTypesC.h>
#include <ogvr/Util/AnnotationMacrosC.h>
#include <ogvr/Util/TimeValueC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup plugin_c_device_api Plugin C API for Devices
    @brief How to create and report from a device in your plugin.
    @ingroup plugin_api
@{
*/

/** @brief Opaque type of a registered device token within the core library.

    Each device you register will be given one of these. You must hold
    on to it in association with that device until the device is no longer
    active, as each call from the device into this C device API will
    require it.
*/
typedef void *OGVR_DeviceToken;

/** @brief Opaque type of a registered message type within the core library.

Common device types will have pre-defined message types, while more specific
or unique devices may need to define their own.
*/
typedef void *OGVR_MessageType;

/** @brief Register (or recall) a message type by name.

@param ctx The plugin registration context received by your entry point
function.
@param name A unique name for the message type. The library makes a copy of this
string.
@param [out] msgtype Will contain the message type identifier you've
registered..
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode
    ogvrDeviceRegisterMessageType(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                                  OGVR_IN_STRZ const char *name,
                                  OGVR_OUT_PTR OGVR_MessageType *msgtype)
        OGVR_FUNC_NONNULL((1, 2, 3));

/** @brief Send a raw bytestream from your device.

    @note The same function is used for synchronous and asynchronous devices:
   the device token is sufficient to determine whether locking is needed.

*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode
    ogvrDeviceSendData(OGVR_INOUT_PTR OGVR_DeviceToken dev,
                       OGVR_IN_PTR OGVR_MessageType msg,
                       OGVR_IN_READS(len) const char *bytestream,
                       OGVR_IN size_t len) OGVR_FUNC_NONNULL((1, 2));

/** @brief Send a raw bytestream from your device, with a known timestamp.

    @note The same function is used for synchronous and asynchronous devices:
    the device token is sufficient to determine whether locking is needed.

*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode ogvrDeviceSendTimestampedData(
    OGVR_INOUT_PTR OGVR_DeviceToken dev,
    OGVR_IN_PTR const struct OGVR_TimeValue *timestamp,
    OGVR_IN_PTR OGVR_MessageType msg, OGVR_IN_READS(len) const char *bytestream,
    OGVR_IN size_t len) OGVR_FUNC_NONNULL((1, 2, 3));

/** @name Synchronous Devices

    Devices declaring themselves to be synchronous must abide by strict rules.
    Their update method is run regularly in the main thread of the device
    system, without the overhead of locking. In exchange, however, the following
    restrictions apply:

    - The update method must complete as quickly as reasonably possible, since
    it adds to overall latency.
    - No data may be sent through the library outside of the update method, and
    the update method should only be called by the core library (not by the
    plugin)

    @{
*/
/** @brief Initialize a synchronous device token.

    This primarily allocates the device token, and does not start reporting.

    @param ctx The plugin registration context received by your entry point
   function.
    @param name A unique name for the device, abiding by the rules for an
   element (directory) in a URL. The library makes a copy of this string.
    @param [out] device Will contain the unique device token assigned to your
   synchronous device.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode
    ogvrDeviceSyncInit(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                       OGVR_IN_STRZ const char *name,
                       OGVR_OUT_PTR OGVR_DeviceToken *device)
        OGVR_FUNC_NONNULL((1, 2, 3));

/** @brief Register the update callback of a synchronous device.

    The callback you provide will be called in the main update loop, potentially
   as soon as this call completes.

    When your callback, a function of type OGVR_SyncDeviceUpdateCallback, is
   invoked, it will receive the same userdata you provide here (if any).

    @param device The device token.
    @param updateCallback The address of your callback function.
    @param userData An opaque pointer that will be returned to you when
    the callback you register here is called. Technically optional, but hard to
    support multiple instances without it.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode ogvrDeviceSyncRegisterUpdateCallback(
    OGVR_INOUT_PTR OGVR_DeviceToken device,
    OGVR_IN OGVR_SyncDeviceUpdateCallback updateCallback,
    OGVR_IN_OPT void *userData OGVR_CPP_ONLY(= NULL)) OGVR_FUNC_NONNULL((1));
/** @} */

/** @name Asynchronous Devices

    These devices are more event-based: either it's convenient for your
    driver to block until full data arrives, or you can't be sure your
    driver can get in and out of an update function very rapidly.

    As a result, devices registered as async have their analog to an update
   method run in a thread of its own, repeatedly as long as the device exists.
    Calls sending data from an async device are automatically made thread-safe.

    @{
*/
/** @brief Initialize an asynchronous device token.

    This primarily allocates the device token, and does not start reporting.

    @param ctx The plugin registration context received by your entry point
   function.
    @param name A unique name for the device, abiding by the rules for an
   element (directory) in a URL. The library makes a copy of this string.
    @param [out] device Will contain the unique device token assigned to your
   asynchronous device.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode
    ogvrDeviceAsyncInit(OGVR_INOUT_PTR OGVR_PluginRegContext ctx,
                        OGVR_IN_STRZ const char *name,
                        OGVR_OUT_PTR OGVR_DeviceToken *device)
        OGVR_FUNC_NONNULL((1, 2, 3));

/** @brief Start the sampling/waiting thread of an asynchronous device.

    The callback you provide will immediately and repeatedly be called in its
   own thread until stopped.

    When your callback, a function of type OGVR_AsyncDeviceWaitCallback, is
   invoked, it will receive the same userdata you provide here (if any).

    @param device Your device token.
    @param waitCallback The address of your callback function.
    @param userData An opaque pointer that will be returned to you when
    the callback you register here is called. Technically optional, but hard to
   support multiple instances without it.
*/
OGVR_PLUGINKIT_EXPORT OGVR_ReturnCode ogvrDeviceAsyncStartWaitLoop(
    OGVR_INOUT_PTR OGVR_DeviceToken device,
    OGVR_IN OGVR_AsyncDeviceWaitCallback waitCallback,
    OGVR_IN_OPT void *userData OGVR_CPP_ONLY(= NULL)) OGVR_FUNC_NONNULL((1));

/** @} */

/** @} */ /* end of group */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
