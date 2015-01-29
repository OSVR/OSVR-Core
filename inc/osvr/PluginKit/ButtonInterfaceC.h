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
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief The integer type specifying the number of button channels you'd like.
*/
typedef OSVR_ChannelCount OSVR_ButtonChanCount;

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
                          OSVR_IN OSVR_ButtonChanCount numChan)
    OSVR_FUNC_NONNULL((1, 2));

OSVR_EXTERN_C_END

#endif
