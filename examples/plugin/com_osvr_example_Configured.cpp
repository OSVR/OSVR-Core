/** @file
    @brief Sample plugin that accepts configuration of its device from the
   server. See osvr_server_config.ConfiguredPlugin.sample.json for how to load a
   plugin like this.

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_example_Configured_json.h"

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>

// Anonymous namespace to avoid symbol collision
namespace {

class ConfiguredDevice {
  public:
    ConfiguredDevice(OSVR_PluginRegContext ctx, double myVal, int sleepTime)
        : m_myVal(myVal), m_sleepTime(sleepTime) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Indicate that we'll want 1 analog channel.
        osvrDeviceAnalogConfigure(opts, &m_analog, 1);

        /// Create the async device token with the options
        m_dev.initAsync(ctx, "MyConfiguredDevice", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_example_Configured_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        /// Because this is an async device, we can block for data,
        /// simulated here with a sleep.
        std::this_thread::sleep_for(std::chrono::seconds(m_sleepTime));

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    double m_myVal;
    int m_sleepTime;
};

class ConfiguredDeviceConstructor {
  public:
    /// @brief This is the required signature for a device instantiation
    /// callback.
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        // Read the JSON data from parameters.
        Json::Value root;
        if (params) {
            Json::Reader r;
            if (!r.parse(params, root)) {
                std::cerr << "Could not parse parameters!" << std::endl;
            }
        }

        if (!root.isMember("value")) {
            // If they configured us but didn't provide a "value" item, warn
            // - pretending that "value" is some important config value.
            // If very important, could just return OSVR_RETURN_FAILURE here.
            std::cerr << "Warning: got configuration, but nothing specified "
                         "for \"value\" - will use default!"
                      << std::endl;
        }

        // Using `get` here instead of `[]` lets us provide a default value.
        double val = root.get("value", 5.0).asDouble();

        // We didn't warn about this one, because we're pretending this is a
        // less important optional parameter.
        int sleepTime = root.get("sleepTime", 1).asInt();

        // OK, now that we have our parameters, create the device.
        osvr::pluginkit::registerObjectForDeletion(
            ctx, new ConfiguredDevice(ctx, val, sleepTime));

        return OSVR_RETURN_SUCCESS;
    }
};
} // namespace

OSVR_PLUGIN(com_osvr_example_Configured) {

    /// Tell the core we're available to create a device object.
    osvr::pluginkit::registerDriverInstantiationCallback(
        ctx, "ConfiguredDevice", new ConfiguredDeviceConstructor);

    return OSVR_RETURN_SUCCESS;
}
