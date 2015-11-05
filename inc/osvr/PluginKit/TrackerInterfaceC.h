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

#ifndef INCLUDED_TrackerInterfaceC_h_GUID_FA1B5B80_FEB5_469D_8F1E_7ED54A27A7AD
#define INCLUDED_TrackerInterfaceC_h_GUID_FA1B5B80_FEB5_469D_8F1E_7ED54A27A7AD

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCTracker Tracker interface (base C API)
    @brief Sending tracker reports from a device in your plugin.
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    a tracker interface.
*/
typedef struct OSVR_TrackerDeviceInterfaceObject *OSVR_TrackerDeviceInterface;

/** @brief Specify that your device will implement the Tracker interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
   lifetime as the device token in order to send messages conforming to a
   tracker interface.

*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_TrackerDeviceInterface *iface)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report the full rigid body pose of a sensor, automatically generating
   a timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendPose(OSVR_IN_PTR OSVR_DeviceToken dev,
                          OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                          OSVR_IN_PTR OSVR_PoseState const *val,
                          OSVR_IN OSVR_ChannelCount sensor)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the full rigid body pose of a sensor, using the supplied
   timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendPoseTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PoseState const *val, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the position of a sensor that doesn't report orientation,
   automatically generating a timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendPosition(OSVR_IN_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                              OSVR_IN_PTR OSVR_PositionState const *val,
                              OSVR_IN OSVR_ChannelCount sensor)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the position of a sensor that doesn't report orientation,
   using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendPositionTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PositionState const *val, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the orientation of a sensor that doesn't report position,
   automatically generating a timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendOrientation(OSVR_IN_PTR OSVR_DeviceToken dev,
                                 OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                                 OSVR_IN_PTR OSVR_OrientationState const *val,
                                 OSVR_IN OSVR_ChannelCount sensor)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the orientation of a sensor that doesn't report position,
   using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendOrientationTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_OrientationState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the linear and angular velocity of a sensor, using the
   supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendVelocityTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_VelocityState const *val, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the linear velocity of a sensor that doesn't report angular
   velocity, using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendLinearVelocityTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_LinearVelocityState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the angular velocity of a sensor that doesn't report linear
   velocity, using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendAngularVelocityTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_AngularVelocityState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the linear and angular Acceleration of a sensor, using the
   supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendAccelerationTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_AccelerationState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the linear acceleration of a sensor that doesn't report
   angular acceleration, using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendLinearAccelerationTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_LinearAccelerationState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the angular acceleration of a sensor that doesn't report
   linear acceleration, using the supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendAngularAccelerationTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_AngularAccelerationState const *val,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
