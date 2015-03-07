/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_DeviceInterfaceC_h_GUID_8B82B108_1B20_4D80_9FE5_C31E424E3652
#define INCLUDED_DeviceInterfaceC_h_GUID_8B82B108_1B20_4D80_9FE5_C31E424E3652

/* Internal Includes */
#include <osvr/PluginKit/Export.h>
#include <osvr/PluginKit/CommonC.h>
#include <osvr/Util/DeviceCallbackTypesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/TimeValueC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stddef.h>

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCDevices Device-related functionality (base C API)
    @brief How to create and report from a device in your plugin.
    @ingroup PluginKit
@{
*/

/** @brief Opaque type of a registered device token within the core library.

    Each device you register will be given one of these. You must hold
    on to it in association with that device until the device is no longer
    active, as each call from the device into this C device API will
    require it.
*/
typedef struct OSVR_DeviceTokenObject *OSVR_DeviceToken;

/** @brief Opaque type of a registered message type within the core library.

    Common device types will have pre-defined message types, while more specific
    or unique devices may need to define their own.
*/
typedef struct OSVR_MessageTypeObject *OSVR_MessageType;

/** @brief Opaque type of a device initialization object.

    When creating a device token that implements one or more existing interface
   types, you'll construct one of these, specify which interfaces you are
   implementing, then pass it to the device token creation.
*/
typedef struct OSVR_DeviceInitObject *OSVR_DeviceInitOptions;

/** @brief Create a OSVR_DeviceInitOptions object.

    There is no corresponding destroy method because it is handed over to a
   device token constructor that takes ownership of it.

    @param ctx The plugin registration context received by your entry point
    function.
*/
OSVR_PLUGINKIT_EXPORT OSVR_DeviceInitOptions
osvrDeviceCreateInitOptions(OSVR_IN_PTR OSVR_PluginRegContext ctx)
    OSVR_FUNC_NONNULL((1));

/** @brief Register (or recall) a message type by name.

    @param ctx The plugin registration context received by your entry point
    function.
    @param name A unique name for the message type. The library makes a copy of
   this string.
    @param [out] msgtype Will contain the message type identifier you've
    registered.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceRegisterMessageType(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                              OSVR_IN_STRZ const char *name,
                              OSVR_OUT_PTR OSVR_MessageType *msgtype)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Send a raw bytestream from your device.

    @note The same function is used for synchronous and asynchronous devices:
   the device token is sufficient to determine whether locking is needed.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceSendData(OSVR_IN_PTR OSVR_DeviceToken dev,
                   OSVR_IN_PTR OSVR_MessageType msg,
                   OSVR_IN_READS(len) const char *bytestream,
                   OSVR_IN size_t len) OSVR_FUNC_NONNULL((1, 2));

/** @brief Send a raw bytestream from your device, with a known timestamp.

    @note The same function is used for synchronous and asynchronous devices:
    the device token is sufficient to determine whether locking is needed.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceSendTimestampedData(OSVR_IN_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR const OSVR_TimeValue *timestamp,
                              OSVR_IN_PTR OSVR_MessageType msg,
                              OSVR_IN_READS(len) const char *bytestream,
                              OSVR_IN size_t len) OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Submit a JSON self-descriptor string for the device.

    Length does not include null terminator.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceSendJsonDescriptor(OSVR_IN_PTR OSVR_DeviceToken dev,
                             OSVR_IN_READS(len) const char *json,
                             OSVR_IN size_t len) OSVR_FUNC_NONNULL((1, 2));

/** @brief Register the update callback of a device.

    The callback you provide will be called potentially as soon as this call
   completes. If you created a Sync device token, it will be run in the main
   update loop. If you created an Async device token, it will be run repeatedly
   in its own thread.

    When your callback, a function of type OSVR_DeviceUpdateCallback, is
    invoked, it will receive the same userdata you provide here (if any).

    @param device The device token.
    @param updateCallback The address of your callback function.
    @param userData An opaque pointer that will be returned to you when
    the callback you register here is called. Technically optional, but hard to
    support multiple instances without it.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode osvrDeviceRegisterUpdateCallback(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN OSVR_DeviceUpdateCallback updateCallback,
    OSVR_IN_OPT void *userData OSVR_CPP_ONLY(= NULL)) OSVR_FUNC_NONNULL((1));

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
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceSyncInit(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                   OSVR_IN_STRZ const char *name,
                   OSVR_OUT_PTR OSVR_DeviceToken *device)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @copydoc osvrDeviceSyncInit
    @brief Initialize a synchronous device token with the options specified.
    @param options The DeviceInitOptions for your device.

    This transfers ownership of the DeviceInitOptions, and all created objects
   associated with it, to the returned device token.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceSyncInitWithOptions(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                              OSVR_IN_STRZ const char *name,
                              OSVR_IN_PTR OSVR_DeviceInitOptions options,
                              OSVR_OUT_PTR OSVR_DeviceToken *device)
    OSVR_FUNC_NONNULL((1, 2, 3, 4));

/** @} */

/** @name Asynchronous Devices

    These devices are more event-based: either it's convenient for your
    driver to block until full data arrives, or you can't be sure your
    driver can get in and out of an update function very rapidly.

    As a result, devices registered as async have their update
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
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceAsyncInit(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                    OSVR_IN_STRZ const char *name,
                    OSVR_OUT_PTR OSVR_DeviceToken *device)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @copydoc osvrDeviceAsyncInit
    @brief Initialize an asynchronous device token with the options specified.
    @param options The DeviceInitOptions for your device.

    This transfers ownership of the DeviceInitOptions, and all created objects
    associated with it, to the returned device token.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceAsyncInitWithOptions(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                               OSVR_IN_STRZ const char *name,
                               OSVR_IN_PTR OSVR_DeviceInitOptions options,
                               OSVR_OUT_PTR OSVR_DeviceToken *device)
    OSVR_FUNC_NONNULL((1, 2, 3, 4));

/** @} */

/** @brief Request a thread sleep for at least the given number of microseconds.
    DO NOT use within a Sync plugin!

    This is just a request for a minimum sleep time - operating system
   scheduling and sleep granularity means that you may end up sleeping for
   longer.
*/
OSVR_PLUGINKIT_EXPORT OSVR_ReturnCode
osvrDeviceMicrosleep(OSVR_IN uint64_t microseconds);

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
