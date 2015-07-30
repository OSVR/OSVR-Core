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
#include <osvr/PluginKit/GestureInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Common/GestureComponent.h>
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/Connection/DeviceInterfaceBase.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_GestureDeviceInterfaceObject
    : public osvr::connection::DeviceInterfaceBase {
    osvr::common::GestureComponent *gesture;
};

OSVR_ReturnCode
osvrDeviceGestureConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_GestureDeviceInterface *iface,
                           OSVR_IN OSVR_ChannelCount numSensors) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceGestureConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceGestureConfigure", iface);
    OSVR_GestureDeviceInterface ifaceObj =
        opts->makeInterfaceObject<OSVR_GestureDeviceInterfaceObject>();
    *iface = ifaceObj;
    auto gesture = osvr::common::GestureComponent::create(numSensors);
    ifaceObj->gesture = gesture.get();
    opts->addComponent(gesture);
    return OSVR_RETURN_SUCCESS;
}

void
osvrDeviceGestureGetID(OSVR_IN_PTR OSVR_GestureDeviceInterface iface,
OSVR_IN_PTR const char *gestureName,
OSVR_IN_PTR OSVR_GestureID *gestureID){

	*gestureID = iface->gesture->getGestureID(gestureName);

}

OSVR_ReturnCode
osvrDeviceGestureReportData(OSVR_IN_PTR OSVR_GestureDeviceInterface iface,
OSVR_IN OSVR_GestureID gestureID,
OSVR_IN_PTR OSVR_GestureState gestureState,
OSVR_IN OSVR_ChannelCount sensor,
OSVR_IN_PTR OSVR_TimeValue const *timestamp){

	auto guard = iface->getSendGuard();
	if (guard->lock()) {
		iface->gesture->sendGestureData(gestureState, gestureID, sensor,
			*timestamp);
		return OSVR_RETURN_SUCCESS;
	}

	return OSVR_RETURN_FAILURE;

}

OSVR_ReturnCode
osvrDeviceGestureReportDataWithName(OSVR_IN_PTR OSVR_GestureDeviceInterface iface,
                            OSVR_IN_PTR const char *gestureName,
                            OSVR_IN_PTR OSVR_GestureState gestureState,
                            OSVR_IN OSVR_ChannelCount sensor,
                            OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->gesture->sendGestureData(gestureState, gestureName, sensor,
                                        *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}