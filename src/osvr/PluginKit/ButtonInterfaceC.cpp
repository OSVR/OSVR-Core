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
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/ButtonServerInterface.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "HandleNullContext.h"
#include "PointerWrapper.h"

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_ButtonDeviceInterfaceObject
    : public PointerWrapper<osvr::connection::ButtonServerInterface> {};

OSVR_ReturnCode
osvrDeviceButtonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_ButtonDeviceInterface *iface,
                          OSVR_IN OSVR_ButtonChanCount numChan) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", iface);
    OSVR_ButtonDeviceInterface ifaceObj = *iface =
        opts->getContext()->registerDataWithGenericDelete(
            new OSVR_ButtonDeviceInterfaceObject);
    opts->setButtons(numChan, ifaceObj->getContainerLocation());

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceButtonSetValue(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                         OSVR_IN_PTR OSVR_ButtonDeviceInterface
                                             iface,
                                         OSVR_IN OSVR_ButtonState val,
                                         OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceButtonSetValueTimestamped(dev, iface, val, chan, &now);
}

OSVR_ReturnCode osvrDeviceButtonSetValueTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_ButtonDeviceInterface iface, OSVR_IN OSVR_ButtonState val,
    OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValueTimestamped", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValueTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValueTimestamped",
                                    timestamp);

    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);

    auto guard = device->getSendGuard();
    if (guard->lock()) {
        bool sendResult = (*iface)->setValue(val, chan, *timestamp);
        return sendResult ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceButtonSetValues(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                          OSVR_IN_PTR OSVR_ButtonDeviceInterface
                                              iface,
                                          OSVR_IN_PTR OSVR_ButtonState val[],
                                          OSVR_IN OSVR_ChannelCount chans) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceButtonSetValuesTimestamped(dev, iface, val, chans, &now);
}

OSVR_ReturnCode osvrDeviceButtonSetValuesTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
    OSVR_IN_PTR OSVR_ButtonState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValuesTimestamped",
                                    dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValuesTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValuesTimestamped",
                                    timestamp);
    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);

    auto guard = device->getSendGuard();
    if (guard->lock()) {
        (*iface)->setValues(val, chans, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}