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

#ifndef INCLUDED_EyeTrackerInterfaceC_h_GUID_F6C50F38_5C4F_41AD_B703_DE8A073F77B3
#define INCLUDED_EyeTrackerInterfaceC_h_GUID_F6C50F38_5C4F_41AD_B703_DE8A073F77B3

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

/** @defgroup PluginKitCEyeTracker EyeTracker interface underlying C API - not
intended for direct usage
@brief Used by the C++ header-only wrappers to send eye tracker reports from a
device in your plugin.
@ingroup PluginKit
@{
*/

OSVR_EXTERN_C_BEGIN

/** @brief Opaque type used in conjunction with a device token to send data on eye
tracker interface (which factors into more interfaces, see implementation).
*/
typedef struct OSVR_EyeTrackerDeviceInterfaceObject *OSVR_EyeTrackerDeviceInterface;

/** @brief Specify that your device will implement the Eye Tracker interface.

@param opts The device init options object.
@param [out] iface An interface object you should retain with the same
lifetime as the device token in order to send messages conforming to an
imaging interface.
@param numSensors The number of eye tracker sensors you will be reporting:
You can have repot 1 - 3 sensors. This 
parameter may be subject to external limitations
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
OSVR_OUT_PTR OSVR_EyeTrackerDeviceInterface *iface OSVR_FUNC_NONNULL((1, 2)));

/** @brief Report gaze position 2D for a sensor (eye)
@param dev Device token
@param iface Eye Tracker interface
@param gazePosition A copy of the eye gaze Position
@param sensor Sensor number
@param timestamp Timestamp correlating to eye data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerReport2DGaze(OSVR_IN_PTR OSVR_DeviceToken dev,
OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
OSVR_IN_PTR OSVR_EyeGazePosition2DReport *gazePosition,
//OSVR_IN OSVR_EyeBlinkState blink,
OSVR_IN_PTR OSVR_TimeValue const *timestamp)
OSVR_FUNC_NONNULL((1, 2, 3));
/** @} */ /* end of group */

/** @brief Report 3D gaze direction for an eye
@param dev Device token
@param iface Eye Tracker interface
@param gazeDirection A copy of the eye gaze Direction
@param sensor Sensor number
@param timestamp Timestamp correlating to eye data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerReport3DGaze(OSVR_IN_PTR OSVR_DeviceToken dev,
OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeDirection,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeBasePoint,
//OSVR_IN OSVR_EyeBlinkState blink,
OSVR_IN_PTR OSVR_TimeValue const *timestamp)
OSVR_FUNC_NONNULL((1, 2, 3, 4));
/** @} */ /* end of group */


/** @brief Report gaze direction for an eye
@param dev Device token
@param iface Eye Tracker interface
@param gazeBasePoint A copy of the eye gaze direction
@param sensor Sensor number
@param timestamp Timestamp correlating to eye data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerReportGazeBasePoint(OSVR_IN_PTR OSVR_DeviceToken dev,
OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeBasePoint,
//OSVR_IN OSVR_EyeBlinkState blink,
OSVR_IN_PTR OSVR_TimeValue const *timestamp)
OSVR_FUNC_NONNULL((1, 2, 3));
/** @} */ /* end of group */

/** @brief Report gaze base point for an eye
@param dev Device token
@param iface Eye Tracker interface
@param gazeBasePoint A copy of the eye gaze base point
@param sensor Sensor number
@param timestamp Timestamp correlating to eye data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerReportGazeDirection(OSVR_IN_PTR OSVR_DeviceToken dev,
OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeDirection,
//OSVR_IN OSVR_EyeBlinkState blink,
OSVR_IN_PTR OSVR_TimeValue const *timestamp)
OSVR_FUNC_NONNULL((1, 2, 3));
/** @} */ /* end of group */


/** @brief Report both 2D and 3D gaze directions for an eye
@param dev Device token
@param iface Eye Tracker interface
@param gazeBasePoint A copy of the eye tracker report
@param sensor Sensor number
@param timestamp Timestamp correlating to eye data.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceEyeTrackerReportGaze(OSVR_IN_PTR OSVR_DeviceToken dev,
OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
OSVR_IN_PTR OSVR_EyeGazePosition2DReport *gazePosition,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeDirection,
OSVR_IN_PTR OSVR_EyeGazeDirectionReport *gazeBasePoint,
//OSVR_IN OSVR_EyeBlinkState blink,
OSVR_IN_PTR OSVR_TimeValue const *timestamp)
OSVR_FUNC_NONNULL((1, 2, 3, 4));
/** @} */ /* end of group */


OSVR_EXTERN_C_END

#endif // INCLUDED_EyeTrackerInterfaceC_h_GUID_F6C50F38_5C4F_41AD_B703_DE8A073F77B3

