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
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
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
#include <ogvr/PluginKit/CommonC.h>

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
typedef void *OGVRDevice;

/** @brief Send a raw bytestream from your device.

    @note The same function is used for synchronous and asynchronous devices:
   the device token is sufficient to determine whether locking is needed.

    @todo include a message type ID
*/
OGVR_PLUGINKIT_EXPORT OGVRPluginReturnCode
    ogvrDeviceSendData(OGVRDevice dev, const char *bytestream, size_t len);

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
    @outparam device Will contain the unique device token assigned to your
   synchronous device.
*/
OGVR_PLUGINKIT_EXPORT OGVRPluginReturnCode
    ogvrDeviceSyncInit(OGVRPluginRegContext ctx, const char *name,
                       OGVRDevice *device);

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
    @outparam device Will contain the unique device token assigned to your
   asynchronous device.
*/
OGVR_PLUGINKIT_EXPORT OGVRPluginReturnCode
    ogvrDeviceAsyncInit(OGVRPluginRegContext ctx, const char *name,
                        OGVRDevice *device);

/** @} */

/** @} */ /* end of group */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
