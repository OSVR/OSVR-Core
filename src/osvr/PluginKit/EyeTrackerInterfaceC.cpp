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
#include <osvr/PluginKit/EyeTrackerInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "PointerWrapper.h"
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/EyeTrackerComponent.h>
#include <osvr/Common/Location2DComponent.h>
#include <osvr/Common/DirectionComponent.h>
#include <osvr/Connection/TrackerServerInterface.h>
#include <osvr/Connection/ButtonServerInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_EyeTrackerDeviceInterfaceObject {
    osvr::common::EyeTrackerComponent *eyetracker;
    osvr::common::Location2DComponent *location;
    osvr::common::DirectionComponent *direction;
    PointerWrapper<osvr::connection::ButtonServerInterface> button;
    PointerWrapper<osvr::connection::TrackerServerInterface> tracker;
    PointerWrapper<OSVR_DeviceTokenObject> token;
};

OSVR_ReturnCode osvrDeviceEyeTrackerConfigure(
    OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
    OSVR_OUT_PTR OSVR_EyeTrackerDeviceInterface *iface,
    OSVR_IN OSVR_ChannelCount numChan) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceEyeTrackerConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceEyeTrackerConfigure", iface);
    OSVR_EyeTrackerDeviceInterface ifaceObj =
        opts->getContext()->registerDataWithGenericDelete(
            new OSVR_EyeTrackerDeviceInterfaceObject);
    *iface = ifaceObj;

    auto location = osvr::common::Location2DComponent::create();
    ifaceObj->location = location.get();
    opts->addComponent(location);

    auto direction = osvr::common::DirectionComponent::create();
    ifaceObj->direction = direction.get();
    opts->addComponent(direction);

    auto eyetracker = osvr::common::EyeTrackerComponent::create();
    ifaceObj->eyetracker = eyetracker.get();
    opts->addComponent(eyetracker);

    opts->setButtons(numChan, ifaceObj->button.getContainerLocation());
    opts->setTracker(ifaceObj->tracker.getContainerLocation());
    opts->addTokenInterest(ifaceObj->token);

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrDeviceEyeTrackerReport2DGaze(
    OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
    OSVR_IN OSVR_EyeGazePosition2DState gazePosition,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {

    auto guard = iface->token->getSendGuard();
    if (guard->lock()) {
        iface->location->sendLocationData(gazePosition, sensor, *timestamp);
        iface->eyetracker->sendNotification(sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceEyeTrackerReport3DGaze(
    OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
    OSVR_IN OSVR_EyeGazeDirectionState gazeDirection,
    OSVR_IN OSVR_EyeGazeBasePoint3DState gazeBasePoint,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->token->getSendGuard();
    if (guard->lock()) {
        iface->direction->sendDirectionData(gazeDirection, sensor, *timestamp);
        iface->tracker->sendReport(gazeBasePoint, sensor, *timestamp);
        iface->eyetracker->sendNotification(sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceEyeTrackerReport3DGazeDirection(
    OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
    OSVR_IN OSVR_EyeGazeDirectionState gazeDirection,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {

    auto guard = iface->token->getSendGuard();
    if (guard->lock()) {
        iface->direction->sendDirectionData(gazeDirection, sensor, *timestamp);
        iface->eyetracker->sendNotification(sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceEyeTrackerReportGaze(
    OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
    OSVR_IN OSVR_EyeGazePosition2DState gazePosition,
    OSVR_IN OSVR_EyeGazeDirectionState gazeDirection,
    OSVR_IN OSVR_EyeGazeBasePoint3DState gazeBasePoint,
    OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {

    auto guard = iface->token->getSendGuard();
    if (guard->lock()) {
        iface->location->sendLocationData(gazePosition, sensor, *timestamp);
        iface->tracker->sendReport(gazeBasePoint, sensor, *timestamp);
        iface->direction->sendDirectionData(gazeDirection, sensor, *timestamp);
        iface->eyetracker->sendNotification(sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrDeviceEyeTrackerReportBlink(
    OSVR_IN_PTR OSVR_EyeTrackerDeviceInterface iface,
    OSVR_IN OSVR_EyeTrackerBlinkState blink, OSVR_IN OSVR_ChannelCount sensor,
    OSVR_IN_PTR OSVR_TimeValue const *timestamp) {

    auto guard = iface->token->getSendGuard();
    if (guard->lock()) {
        iface->button->setValue(blink, sensor, *timestamp);
        iface->eyetracker->sendNotification(sensor, *timestamp);
    }
    return OSVR_RETURN_SUCCESS;
}
