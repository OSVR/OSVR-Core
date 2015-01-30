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
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/AnalogServerInterface.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_AnalogDeviceInterfaceObject {
    OSVR_AnalogDeviceInterfaceObject() : realIface(nullptr) {}
    osvr::connection::AnalogServerInterface *realIface;
};

OSVR_ReturnCode
osvrDeviceAnalogConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_AnalogDeviceInterface *iface,
                          OSVR_IN OSVR_ChannelCount numChan) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogConfigure", iface);
    OSVR_AnalogDeviceInterface ifaceObj =
        opts->getContext()->registerDataWithGenericDelete(
            new OSVR_AnalogDeviceInterfaceObject);
    *iface = ifaceObj;
    opts->setAnalogs(numChan, &(ifaceObj->realIface));
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceAnalogSetValue(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                         OSVR_IN_PTR OSVR_AnalogDeviceInterface
                                             iface,
                                         OSVR_IN OSVR_AnalogState val,
                                         OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceAnalogSetValueTimestamped(dev, iface, val, chan, &now);
}

OSVR_ReturnCode osvrDeviceAnalogSetValueTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface, OSVR_IN OSVR_AnalogState val,
    OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValueTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    timestamp);
    bool sendResult = false;
    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);
    bool callResult = device->callWhenSafeToSend([&] {
        sendResult = iface->realIface->setValue(val, chan, *timestamp);
    });
    return (callResult && sendResult) ? OSVR_RETURN_SUCCESS
                                      : OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceAnalogSetValues(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                                          OSVR_IN_PTR OSVR_AnalogDeviceInterface
                                              iface,
                                          OSVR_IN_PTR OSVR_AnalogState val[],
                                          OSVR_IN OSVR_ChannelCount chans) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceAnalogSetValuesTimestamped(dev, iface, val, chans, &now);
}

OSVR_ReturnCode osvrDeviceAnalogSetValuesTimestamped(
    OSVR_INOUT_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface,
    OSVR_IN_PTR OSVR_AnalogState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    timestamp);
    osvr::connection::DeviceToken *device =
        static_cast<osvr::connection::DeviceToken *>(dev);
    bool callResult = device->callWhenSafeToSend(
        [&] { iface->realIface->setValues(val, chans, *timestamp); });
    return callResult ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}