/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
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
#include <ogvr/PluginKit/DeviceInterfaceC.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_PluginReturnCode ogvrDeviceSendData(OGVR_DeviceToken dev,
                                         const char *bytestream, size_t len) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE(
        "In ogvrDeviceSendData, trying to send a message of length " << len);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceSyncInit(OGVR_PluginRegContext ctx,
                                         const char *name,
                                         OGVR_DeviceToken *device) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncInit for a device named " << name);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceSyncRegisterUpdateCallback(
    OGVR_DeviceToken device, OGVR_SyncDeviceUpdateCallback updateCallback,
    void *userData) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceSyncRegisterUpdateCallback");
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode ogvrDeviceAsyncInit(OGVR_PluginRegContext ctx,
                                          const char *name,
                                          OGVR_DeviceToken *device) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncInit for a device named " << name);
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PluginReturnCode
ogvrDeviceAsyncStartWaitLoop(OGVR_DeviceToken device,
                             OGVR_AsyncDeviceWaitCallback waitCallback,
                             void *userData) {
    /// @todo implement - replace stub
    OGVR_DEV_VERBOSE("In ogvrDeviceAsyncStartWaitLoop");
    return OGVR_PLUGIN_SUCCESS;
}