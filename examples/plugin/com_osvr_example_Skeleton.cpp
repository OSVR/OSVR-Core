/** @date 2015
    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics Inc.
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
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/SkeletonInterfaceC.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_example_Skeleton_json.h"

// Library/third-party includes

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

// Anonymous namespace to avoid symbol collision
namespace {

class SkeletonDevice {
  public:
    SkeletonDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceSkeletonConfigure(opts, &m_skeleton, com_osvr_example_Skeleton_json, 2);
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the device token with the options
        m_dev.initSync(ctx, "Skeleton", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_example_Skeleton_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

        std::this_thread::sleep_for(std::chrono::milliseconds(
            250)); // Simulate waiting a quarter second for data.

        // add reporting of pre-recorded finger positions to send
        // sensible tracker reports

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_SkeletonDeviceInterface m_skeleton;
    OSVR_TrackerDeviceInterface m_tracker;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        /// we always detect device in sample plugin
        m_found = true;

        std::cout << "PLUGIN: We have detected Skeleton device! " << std::endl;
        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(ctx,
                                                   new SkeletonDevice(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_Skeleton) {

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
