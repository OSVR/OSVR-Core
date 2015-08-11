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

#ifndef INCLUDED_GestureInterfaceC_h_GUID_1BED0900_2C34_47B4_1B62_169A0E3E80D4
#define INCLUDED_GestureInterfaceC_h_GUID_1BED0900_2C34_47B4_1B62_169A0E3E80D4

// Internal Includes
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes

OSVR_EXTERN_C_BEGIN

/** @brief Opaque type used in conjunction with a device token to send data on
 * Gesture Interface
*/
typedef struct OSVR_GestureDeviceInterfaceObject *OSVR_GestureDeviceInterface;

/** @brief Specify that your device will implement the Gesture interface.

@param opts The device init options object.
@param [out] iface An interface object you should retain with the same
lifetime as the device token in order to send messages conforming to an
Gesture interface.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceGestureConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_GestureDeviceInterface *iface)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Obtain an ID for a given gesture name
@param iface GestureInterface
@param gestureName String name of gesture
@param gestureID pointer to an id variable
*/
OSVR_PLUGINKIT_EXPORT
void osvrDeviceGestureGetID(OSVR_IN_PTR OSVR_GestureDeviceInterface iface,
                            OSVR_IN_PTR const char *gestureName,
                            OSVR_IN_PTR OSVR_GestureID *gestureID)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report data for a specific sensor.
@param iface Gesture interface
@param gestureID Pre-fetched ID of the gesture
@param gestureState Current state of gesture (In process vs Completed)
@param sensor Sensor number
@param timestamp Timestamp correlating to Gesture data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceGestureReportData(OSVR_IN_PTR OSVR_GestureDeviceInterface iface,
                            OSVR_IN OSVR_GestureID gestureID,
                            OSVR_IN_PTR OSVR_GestureState gestureState,
                            OSVR_IN OSVR_ChannelCount sensor,
                            OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 5));

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif // INCLUDED_GestureInterfaceC_h_GUID_1BED0900_2C34_47B4_1B62_169A0E3E80D4
