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
#include "HandleNullContext.h"
#include <osvr/Connection/ButtonServerInterface.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/DeviceInterfaceBase.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/Util/PointerWrapper.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_ButtonDeviceInterfaceObject
    : public osvr::connection::DeviceInterfaceBase {
    osvr::util::PointerWrapper<osvr::connection::ButtonServerInterface> button;
};

OSVR_ReturnCode
osvrDeviceButtonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                          OSVR_OUT_PTR OSVR_ButtonDeviceInterface *iface,
                          OSVR_IN OSVR_ChannelCount numChan) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonConfigure", iface);
    OSVR_ButtonDeviceInterface ifaceObj =
        opts->makeInterfaceObject<OSVR_ButtonDeviceInterfaceObject>();
    *iface = ifaceObj;
    opts->setButtons(numChan, ifaceObj->button);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceButtonSetValue(OSVR_IN_PTR OSVR_DeviceToken dev,
                         OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
                         OSVR_IN OSVR_ButtonState val,
                         OSVR_IN OSVR_ChannelCount chan) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceButtonSetValueTimestamped(dev, iface, val, chan, &now);
}

OSVR_ReturnCode osvrDeviceButtonSetValueTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken, OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
    OSVR_IN OSVR_ButtonState val, OSVR_IN OSVR_ChannelCount chan,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValueTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValueTimestamped",
                                    timestamp);

    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        bool sendResult = iface->button->setValue(val, chan, *timestamp);
        return sendResult ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode
osvrDeviceButtonSetValues(OSVR_INOUT_PTR OSVR_DeviceToken dev,
                          OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
                          OSVR_IN_PTR OSVR_ButtonState val[],
                          OSVR_IN OSVR_ChannelCount chans) {
    OSVR_TimeValue now;
    osvrTimeValueGetNow(&now);
    return osvrDeviceButtonSetValuesTimestamped(dev, iface, val, chans, &now);
}

OSVR_ReturnCode osvrDeviceButtonSetValuesTimestamped(
    OSVR_IN_PTR OSVR_DeviceToken, OSVR_IN_PTR OSVR_ButtonDeviceInterface iface,
    OSVR_IN_PTR OSVR_ButtonState val[], OSVR_IN OSVR_ChannelCount chans,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValuesTimestamped",
                                    iface);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceButtonSetValuesTimestamped",
                                    timestamp);

    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->button->setValues(val, chans, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}
