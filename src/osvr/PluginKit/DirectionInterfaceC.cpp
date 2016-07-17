/** @file
    @brief Implementation

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

// Internal Includes
#include "HandleNullContext.h"
#include <osvr/Common/DirectionComponent.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/DeviceInterfaceBase.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/PluginKit/DirectionInterfaceC.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_DirectionDeviceInterfaceObject
    : public osvr::connection::DeviceInterfaceBase {
    osvr::common::DirectionComponent *direction;
};

OSVR_ReturnCode
osvrDeviceDirectionConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                             OSVR_OUT_PTR OSVR_DirectionDeviceInterface *iface,
                             OSVR_IN OSVR_ChannelCount numSensors) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceDirectionConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceDirectionConfigure", iface);
    OSVR_DirectionDeviceInterface ifaceObj =
        opts->makeInterfaceObject<OSVR_DirectionDeviceInterfaceObject>();
    *iface = ifaceObj;

    auto direction = osvr::common::DirectionComponent::create(numSensors);
    ifaceObj->direction = direction.get();
    opts->addComponent(direction);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceDirectionReportData(OSVR_IN_PTR OSVR_DirectionDeviceInterface iface,
                              OSVR_IN_PTR OSVR_DirectionState directionData,
                              OSVR_IN OSVR_ChannelCount sensor,
                              OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->direction->sendDirectionData(directionData, sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}