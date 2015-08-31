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

#ifndef INCLUDED_DirectionInterfaceC_h_GUID_5F33787D_F830_4C8F_D066_B649857F3395
#define INCLUDED_DirectionInterfaceC_h_GUID_5F33787D_F830_4C8F_D066_B649857F3395

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCDirection 3D direction interface (base C API)
    @brief Sending 3D direction from a device in your plugin.
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    3D Direction interface.
*/
typedef struct OSVR_DirectionDeviceInterfaceObject
    *OSVR_DirectionDeviceInterface;

/** @brief Specify that your device will implement the Direction interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
    lifetime as the device token in order to send messages conforming to an
    Direction interface.
    @param numSensors The number of sensors you will be reporting Direction data
    for. You can report 1+ sensors. This parameter may be subject to external
    limitations.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceDirectionConfigure(
    OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
    OSVR_OUT_PTR OSVR_DirectionDeviceInterface *iface,
    OSVR_IN OSVR_ChannelCount numSensors OSVR_CPP_ONLY(= 1))
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report data for a specific sensor.

    @param iface Direction interface
    @param directionData Copy of 3D Direction data
    @param sensor Sensor number
    @param timestamp Timestamp correlating to 3D direction data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceDirectionReportData(
    OSVR_IN_PTR OSVR_DirectionDeviceInterface iface,
    OSVR_IN OSVR_DirectionState directionData, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) OSVR_FUNC_NONNULL((1, 4));
/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif // INCLUDED_DirectionInterfaceC_h_GUID_5F33787D_F830_4C8F_D066_B649857F3395
