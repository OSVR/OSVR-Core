/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ImagingInterfaceC_h_GUID_5B6CAEED_5DE5_4CA6_657B_B0DCB5F7DA5C
#define INCLUDED_ImagingInterfaceC_h_GUID_5B6CAEED_5DE5_4CA6_657B_B0DCB5F7DA5C

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ImagingReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

/** @defgroup PluginKitCImaging Imaging interface underlying C API - not
   intended for direct usage
    @brief Used by the C++ header-only wrappers to send image reports from a
   device in your plugin.
    @ingroup PluginKit
    @{
*/

OSVR_EXTERN_C_BEGIN

/** @brief Opaque type used in conjunction with a device token to send data on
    an imaging interface.
*/
typedef struct OSVR_ImagingDeviceInterfaceObject *OSVR_ImagingDeviceInterface;

/** @brief Specify that your device will implement the Imaging interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
    lifetime as the device token in order to send messages conforming to an
    imaging interface.
    @param numSensors The number of imaging sensors you will be reporting:
   usually just 1 (multiple channels per sensor supported). This parameter may
   be subject to external limitations.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceImagingConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_ImagingDeviceInterface *iface,
                           OSVR_IN OSVR_ChannelCount numSensors
                               OSVR_CPP_ONLY(= 1)) OSVR_FUNC_NONNULL((1, 2));

/** @brief Report a frame for a sensor.
    @param dev Device token
    @param iface Imaging interface
    @param metadata Image metadata
    @param imageData A pointer to a copy of the image data, will be destroyed
   when no longer needed using OpenCV deallocation functions.
    @param sensor Sensor number, usually 0
    @param timestamp Timestamp correlating to frame.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceImagingReportFrame(OSVR_IN_PTR OSVR_DeviceToken dev,
                             OSVR_IN_PTR OSVR_ImagingDeviceInterface iface,
                             OSVR_IN OSVR_ImagingMetadata metadata,
                             OSVR_IN_PTR void *imageData,
                             OSVR_IN OSVR_ChannelCount sensor,
                             OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 4, 6));
/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
