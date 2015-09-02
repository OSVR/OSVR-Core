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

/** @defgroup PluginKitCImaging Imaging interface (underlying C API)

    @brief Used by the C++ header-only wrappers to send image reports from a
    device in your plugin. **Not intended for direct usage if it can be
    avoided** due to lifetime management and datatypes.

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

/** @brief Report a frame for a sensor. Takes ownership of the buffer and
    **frees it with the OpenCV deallocation functions** when done, so only
    pass in memory allocated by the matching version of OpenCV.

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
                             OSVR_IN_PTR OSVR_ImageBufferElement *imageData,
                             OSVR_IN OSVR_ChannelCount sensor,
                             OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 4, 6));
/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
