/** @date 2016
    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics Inc.
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
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "org_osvr_example_Tracker_json.h"

// Library/third-party includes

// Standard includes
#include <chrono>
#include <thread>

// Anonymous namespace to avoid symbol collision
namespace {

class TrackerDevice {
  public:
    TrackerDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // configure our tracker
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the device token with the options
        m_dev.initAsync(ctx, "Tracker", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_example_Tracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

        std::this_thread::sleep_for(std::chrono::milliseconds(
            5)); // Simulate waiting a quarter second for data.

        OSVR_TimeValue now;
        osvrTimeValueGetNow(&now);
        double t = 5.0 * ((double)now.seconds + ((double)now.microseconds / 1000000.0));

        /// Report the identity pose for sensor 0
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        pose.translation.data[0] = std::sin(t) * 0.25;
        pose.translation.data[1] = std::cos(t + 0.5) * 0.25;
        pose.translation.data[2] = std::sin(t + 0.25) * 0.25;
        osvrDeviceTrackerSendPose(m_dev, m_tracker, &pose, 0);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        /// we always detect device in sample plugin
        m_found = true;

        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(ctx, new TrackerDevice(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};
} // namespace

OSVR_PLUGIN(org_osvr_example_Tracker) {

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
