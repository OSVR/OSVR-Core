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
#include <osvr/PluginKit/LocomotionInterfaceC.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "HandleNullContext.h"
#include <osvr/Util/PointerWrapper.h>
#include <osvr/Common/LocomotionComponent.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Connection/DeviceInterfaceBase.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_LocomotionDeviceInterfaceObject
    : public osvr::connection::DeviceInterfaceBase {
    osvr::common::LocomotionComponent *locomotion;
};

OSVR_ReturnCode osvrDeviceLocomotionConfigure(
    OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
    OSVR_OUT_PTR OSVR_LocomotionDeviceInterface *iface) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceLocomotionConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceLocomotionConfigure", iface);
    OSVR_LocomotionDeviceInterface ifaceObj =
        opts->makeInterfaceObject<OSVR_LocomotionDeviceInterfaceObject>();
    *iface = ifaceObj;
    auto locomotion = osvr::common::LocomotionComponent::create();
    ifaceObj->locomotion = locomotion.get();
    opts->addComponent(locomotion);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceLocomotionReportNaviVelocity(
    OSVR_IN_PTR OSVR_LocomotionDeviceInterface iface,
    OSVR_IN OSVR_NaviVelocityState naviVelocity,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->locomotion->sendNaviVelocityData(naviVelocity, sensor,
                                                *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceLocomotionReportNaviPosition(
    OSVR_IN_PTR OSVR_LocomotionDeviceInterface iface,
    OSVR_IN OSVR_NaviPositionState naviPosition,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->locomotion->sendNaviPositionData(naviPosition, sensor,
                                                *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}
