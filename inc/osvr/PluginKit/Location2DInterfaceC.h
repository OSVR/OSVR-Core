/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_Location2DInterfaceC_h_GUID_CE39188E_A47A_4085_DAFB_80AB43571F0A
#define INCLUDED_Location2DInterfaceC_h_GUID_CE39188E_A47A_4085_DAFB_80AB43571F0A

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCLocation2D 2D location interface (base C API)
    @brief Sending a normalized 2D location from a device in your plugin.
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    2D Location interface
*/
typedef struct OSVR_Location2D_DeviceInterfaceObject
    *OSVR_Location2D_DeviceInterface;

/** @brief Specify that your device will implement the Location2D interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
    lifetime as the device token in order to send messages conforming to an
    Location2D interface.
    @param numSensors The number of sensors you will be reporting 2D Location
    data: You can report 1+ sensors. This parameter may be subject to external
    limitations
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceLocation2DConfigure(
    OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
    OSVR_OUT_PTR OSVR_Location2D_DeviceInterface *iface,
    OSVR_IN OSVR_ChannelCount numSensors OSVR_CPP_ONLY(= 1))
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report data for a specific sensor.
    @param iface Location2D interface
    @param locationData Copy of 2D Location data
    @param sensor Sensor number
    @param timestamp Timestamp correlating to 2D location data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceLocation2DReportData(
    OSVR_IN_PTR OSVR_Location2D_DeviceInterface iface,
    OSVR_IN OSVR_Location2DState locationData, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) OSVR_FUNC_NONNULL((1, 4));
/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif // INCLUDED_Location2DInterfaceC_h_GUID_CE39188E_A47A_4085_DAFB_80AB43571F0A
