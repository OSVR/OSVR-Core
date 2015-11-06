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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/SkeletonInterfaceC.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "HandleNullContext.h"
#include <osvr/Util/PointerWrapper.h>
#include <osvr/Common/SkeletonComponent.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Connection/DeviceInterfaceBase.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_SkeletonDeviceInterfaceObject
    : public osvr::connection::DeviceInterfaceBase {
    osvr::common::SkeletonComponent *skeleton;
};

OSVR_ReturnCode
osvrDeviceSkeletonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                            OSVR_OUT_PTR OSVR_SkeletonDeviceInterface *iface,
                            OSVR_IN_READS(len) const char *jsonDescriptor,
                            OSVR_IN OSVR_ChannelCount numSensors) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSkeletonConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceSkeletonConfigure", iface);
    OSVR_SkeletonDeviceInterface ifaceObj =
        opts->makeInterfaceObject<OSVR_SkeletonDeviceInterfaceObject>();
    *iface = ifaceObj;
    auto skeleton = osvr::common::SkeletonComponent::create(jsonDescriptor, numSensors);
    ifaceObj->skeleton = skeleton.get();
    opts->addComponent(skeleton);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceSkeletonComplete(OSVR_IN_PTR OSVR_SkeletonDeviceInterface iface,
                           OSVR_IN OSVR_ChannelCount sensor,
                           OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->skeleton->sendNotification(sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode
osvrDeviceSkeletonUpdateSpec(OSVR_IN_PTR OSVR_SkeletonDeviceInterface iface,
                             OSVR_IN_READS(len) const char *spec) {

    auto guard = iface->getSendGuard();
    if (guard->lock()) {
        iface->skeleton->sendArticulationSpec(spec);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}
