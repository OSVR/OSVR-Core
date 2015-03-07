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

#ifndef INCLUDED_AnalogInterfaceC_h_GUID_394913C6_BACB_4A85_EEF8_6765C82907E7
#define INCLUDED_AnalogInterfaceC_h_GUID_394913C6_BACB_4A85_EEF8_6765C82907E7

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCAnalog Analog channel interface (base C API)
    @brief Sending analog reports from a device in your plugin.
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    an analog interface.
*/
typedef struct OSVR_AnalogDeviceInterfaceObject *OSVR_AnalogDeviceInterface;

/** @brief Specify that your device will implement the Analog interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
   lifetime as the device token in order to send messages conforming to an
   analog interface.
    @param numChan The number of channels you will be reporting. This parameter
   may be subject to external limitations (presently 128).

*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceAnalogConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_AnalogDeviceInterface *iface,
                          OSVR_IN OSVR_ChannelCount numChan)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report the value of a single channel.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceAnalogSetValue(OSVR_IN_PTR OSVR_DeviceToken dev,
                                         OSVR_IN_PTR OSVR_AnalogDeviceInterface
                                             iface,
                                         OSVR_IN OSVR_AnalogState val,
                                         OSVR_IN OSVR_ChannelCount chan)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report the value of a single channel with the supplied timestamp
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceAnalogSetValueTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface, OSVR_IN OSVR_AnalogState val,
    OSVR_IN OSVR_ChannelCount chan, OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 5));

/** @brief Report the value of multiple channels
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceAnalogSetValues(OSVR_IN_PTR OSVR_DeviceToken dev,
                                          OSVR_IN_PTR OSVR_AnalogDeviceInterface
                                              iface,
                                          OSVR_IN_PTR OSVR_AnalogState val[],
                                          OSVR_IN OSVR_ChannelCount chans)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the value of multiple channels with the supplied timestamp
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceAnalogSetValuesTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface,
    OSVR_IN_PTR OSVR_AnalogState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
