/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

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

// Internal Includes
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/AnalogServerInterface.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Util/PointerWrapper.h>
#include "HandleNullContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_AnalogDeviceInterfaceObject
    : public osvr::util::PointerWrapper<
          osvr::connection::AnalogServerInterface> {};

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
    opts->setAnalogs(numChan, ifaceObj->getContainerLocation());
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceAnalogSetValue(OSVR_IN_PTR OSVR_DeviceToken dev,
                                         OSVR_IN_PTR OSVR_AnalogDeviceInterface
                                             iface,
                                         OSVR_IN OSVR_AnalogState val,
                                         OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceAnalogSetValueTimestamped(dev, iface, val, chan, &now);
}

OSVR_ReturnCode osvrDeviceAnalogSetValueTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface, OSVR_IN OSVR_AnalogState val,
    OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValueTimestamped", dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValueTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValueTimestamped",
                                    timestamp);

    auto guard = dev->getSendGuard();
    if (guard->lock()) {
        bool sendResult = (*iface)->setValue(val, chan, *timestamp);
        return sendResult ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_FAILURE;
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
    OSVR_IN_PTR OSVR_DeviceToken dev,
    OSVR_IN_PTR OSVR_AnalogDeviceInterface iface,
    OSVR_IN_PTR OSVR_AnalogState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    dev);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceAnalogSetValuesTimestamped",
                                    timestamp);

    auto guard = dev->getSendGuard();
    if (guard->lock()) {
        (*iface)->setValues(val, chans, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}
