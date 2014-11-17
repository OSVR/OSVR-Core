/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
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

#ifndef INCLUDED_AnalogInterfaceC_h_GUID_394913C6_BACB_4A85_EEF8_6765C82907E7
#define INCLUDED_AnalogInterfaceC_h_GUID_394913C6_BACB_4A85_EEF8_6765C82907E7

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief The integer type specifying the number of channels you'd like for
 * your analog. */
typedef uint32_t OSVR_AnalogChanCount;

/** @brief Specify that your device will implement the Analog interface.

    @param opts The device init options object.
    @param numChan The number of channels you will be reporting. This parameter
   may be subject to external limitations (presently 128).

*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceAnalogConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions
                                              opts,
                                          OSVR_IN OSVR_AnalogChanCount numChan)
    OSVR_FUNC_NONNULL((1));

OSVR_EXTERN_C_END

#endif
