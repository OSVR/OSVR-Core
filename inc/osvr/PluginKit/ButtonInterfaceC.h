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
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ButtonInterfaceC_h_GUID_64F54A8F_976B_4A87_7BEA_D5CE628B3E6C
#define INCLUDED_ButtonInterfaceC_h_GUID_64F54A8F_976B_4A87_7BEA_D5CE628B3E6C

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCButton Button channel interface (base C API)
    @brief Sending button reports from a device in your plugin.
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    a button interface.
*/
typedef struct OSVR_ButtonDeviceInterfaceObject *OSVR_ButtonDeviceInterface;

/** @brief Specify that your device will implement the Button interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
   lifetime as the device token in order to send messages conforming to a button
   interface.
    @param numChan The number of channels you will be reporting. This parameter
    may be subject to external limitations (presently 256).

*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceButtonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_ButtonDeviceInterface *iface,
                          OSVR_IN OSVR_ChannelCount numChan)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report the value of a single channel.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceButtonSetValue(OSVR_IN_PTR OSVR_DeviceToken dev,
                                         OSVR_IN_PTR OSVR_ButtonDeviceInterface
                                             iface,
                                         OSVR_IN OSVR_ButtonState val,
                                         OSVR_IN OSVR_ChannelCount chan)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report the value of a single channel with the supplied timestamp
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceButtonSetValueTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_ButtonDeviceInterface iface, OSVR_IN OSVR_ButtonState val,
    OSVR_IN OSVR_ChannelCount chan, OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 5));

/** @brief Report the value of multiple channels
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceButtonSetValues(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                          OSVR_IN_PTR OSVR_ButtonDeviceInterface
                                              iface,
                                          OSVR_IN_PTR OSVR_ButtonState val[],
                                          OSVR_IN OSVR_ChannelCount chans)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the value of multiple channels with the supplied timestamp
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceButtonSetValuesTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
    OSVR_IN_PTR OSVR_ButtonState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
