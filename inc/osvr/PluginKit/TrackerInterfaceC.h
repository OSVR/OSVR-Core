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
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
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

/** @brief Opaque type used in conjunction with a device token to send data on
    a button interface.
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

/** @brief Report the full rigid body pose of a sensor.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendPose(OSVR_IN_PTR OSVR_DeviceToken dev,
                          OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                          OSVR_IN_PTR OSVR_PoseState const *val,
                          OSVR_IN OSVR_ChannelCount chan)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the full rigid body pose of a sensor with the supplied
    timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendPoseTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PoseState const *val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the position of a position-only sensor.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendPosition(OSVR_IN_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                              OSVR_IN_PTR OSVR_PositionState const *val,
                              OSVR_IN OSVR_ChannelCount chan)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the position of a position-only sensor with the supplied
    timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendPositionTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PositionState const *val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

/** @brief Report the orientation of an orientation-only sensor.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceTrackerSendPosition(OSVR_IN_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                              OSVR_IN_PTR OSVR_PositionState const *val,
                              OSVR_IN OSVR_ChannelCount chan)
    OSVR_FUNC_NONNULL((1, 2, 3));

/** @brief Report the orientation of an orientation-only sensor with the
    supplied timestamp.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode osvrDeviceTrackerSendPositionTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PositionState const *val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 2, 3, 5));

OSVR_EXTERN_C_END

#endif
