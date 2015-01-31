/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/Connection/TrackerServerInterface.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "HandleNullContext.h"
#include "PointerWrapper.h"

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_TrackerDeviceInterfaceObject
    : public PointerWrapper<osvr::connection::TrackerServerInterface> {};

OSVR_ReturnCode
osvrDeviceTrackerConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_TrackerDeviceInterface *iface) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceTrackerConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceTrackerConfigure", iface);
    OSVR_TrackerDeviceInterface ifaceObj = *iface =
        opts->getContext()->registerDataWithGenericDelete(
            new OSVR_TrackerDeviceInterfaceObject);
    opts->setTracker(ifaceObj->getContainerLocation());
    return OSVR_RETURN_SUCCESS;
}

template <typename StateType>
static inline OSVR_ReturnCode
osvrTrackerSend(const char method[], OSVR_DeviceToken dev,
                OSVR_TrackerDeviceInterface iface, StateType const *val,
                OSVR_ChannelCount chan, OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT(method, dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT(method, iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT(method, timestamp);

    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);

    auto guard = device->getSendGuard();
    if (guard->lock()) {
        (*iface)->sendReport(*val, chan, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}
OSVR_ReturnCode
osvrDeviceTrackerSendPose(OSVR_IN_PTR OSVR_DeviceToken dev,
                          OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                          OSVR_IN_PTR OSVR_PoseState const *val,
                          OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);

    return osvrDeviceTrackerSendPoseTimestamped(dev, iface, val, chan, &now);
}

OSVR_ReturnCode osvrDeviceTrackerSendPoseTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PoseState const *val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    return osvrTrackerSend("osvrDeviceTrackerSendPoseTimestamped", dev, iface,
                           val, chan, timestamp);
}

OSVR_ReturnCode
osvrDeviceTrackerSendPosition(OSVR_IN_PTR OSVR_DeviceToken dev,
                              OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                              OSVR_IN_PTR OSVR_PositionState const *val,
                              OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);

    return osvrDeviceTrackerSendPositionTimestamped(dev, iface, val, chan,
                                                    &now);
}

OSVR_ReturnCode osvrDeviceTrackerSendPositionTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_PositionState const *val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    return osvrTrackerSend("osvrDeviceTrackerSendPoseTimestamped", dev, iface,
                           val, chan, timestamp);
}

OSVR_ReturnCode
osvrDeviceTrackerSendOrientation(OSVR_IN_PTR OSVR_DeviceToken dev,
                                 OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
                                 OSVR_IN_PTR OSVR_OrientationState const *val,
                                 OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);

    return osvrDeviceTrackerSendOrientationTimestamped(dev, iface, val, chan,
                                                       &now);
}

OSVR_ReturnCode osvrDeviceTrackerSendOrientationTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_TrackerDeviceInterface iface,
    OSVR_IN_PTR OSVR_OrientationState const *val,
    OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    return osvrTrackerSend("osvrDeviceTrackerSendPoseTimestamped", dev, iface,
                           val, chan, timestamp);
}