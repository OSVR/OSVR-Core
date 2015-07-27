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

#ifndef INCLUDED_LocomotionInterfaceC_h_GUID_2458F99D_C36A_4F92_93A4_ABFD224F74AB
#define INCLUDED_LocomotionInterfaceC_h_GUID_2458F99D_C36A_4F92_93A4_ABFD224F74AB

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCLocomotion Locomotion interface (base C API)
@brief Sending locomotion reports from a device in your plugin.
@ingroup PluginKit
@{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
 * Locomotion Interface
*/
typedef struct OSVR_LocomotionDeviceInterfaceObject *
    OSVR_LocomotionDeviceInterface;

/** @brief Specify that your device will implement the Locomotion interface.

@param opts The device init options object.
@param [out] iface An interface object you should retain with the same
lifetime as the device token in order to send messages conforming to an
Locomotion interface.
@param numSensors The number of sensors you will be reporting Locomotion data :
You can report for 1 sensor per device.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceLocomotionConfigure(
    OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
    OSVR_OUT_PTR OSVR_LocomotionDeviceInterface *iface)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report data for a specific sensor.
@param dev Device token
@param iface Locomotion Interface
@param naviVelocity pointer to navigation velocity
@param sensor Sensor number
@param timestamp Timestamp correlating to navigation velocity
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceLocomotionReportNaviVelocity(
    OSVR_IN_PTR OSVR_LocomotionDeviceInterface iface,
    OSVR_IN_PTR OSVR_NaviVelocityState naviVelocity,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) OSVR_FUNC_NONNULL((1, 4));
/**

/** @brief Report data for a specific sensor.
@param dev Device token
@param iface Locomotion Interface
@param naviPosition pointer to navigation Position
@param sensor Sensor number
@param timestamp Timestamp correlating to navigation position
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceLocomotionReportNaviPosition(
    OSVR_IN_PTR OSVR_LocomotionDeviceInterface iface,
    OSVR_IN_PTR OSVR_NaviPositionState naviPosition,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) OSVR_FUNC_NONNULL((1, 4));
/**

@} */ /* end of group */

OSVR_EXTERN_C_END

#endif // INCLUDED_LocomotionInterfaceC_h_GUID_2458F99D_C36A_4F92_93A4_ABFD224F74AB
