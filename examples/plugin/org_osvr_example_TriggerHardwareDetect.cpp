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
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/Logger.h>

// Library/third-party includes

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>


// Anonymous namespace to avoid symbol collision
namespace {

/**
 * @brief This fake device just triggers an autodetect every few seconds.
 */
class FakeDevice {
  public:
    FakeDevice(OSVR_PluginRegContext ctx) : m_context(ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // configure our tracker
        osvrDeviceButtonConfigure(opts, &m_button, 1);

        /// Create the device token with the options
        m_dev.initAsync(ctx, "Hardware detection trigger", opts);

        /// Send JSON descriptor
        //m_dev.sendJsonDescriptor("{}");

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        osvr::pluginkit::triggerHardwareDetect(m_context);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    OSVR_PluginRegContext m_context;
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ButtonDeviceInterface m_button;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false), m_count(0), m_mutex()
    {
        logger_ = osvr::util::log::make_logger("org_osvr_example_TriggerHardwareDetect");
    }

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        std::lock_guard<std::mutex> guard(m_mutex);

        m_count++;
        logger_->info() << "Hardware detection triggered " << m_count << " times.";

        if (m_found)
            return OSVR_RETURN_SUCCESS;

        osvr::pluginkit::registerObjectForDeletion(ctx, new FakeDevice(ctx));
        m_found = true;

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
    int m_count;
    std::mutex m_mutex;
    osvr::util::log::LoggerPtr logger_;
};

} // namespace

OSVR_PLUGIN(org_osvr_example_TriggerHardwareDetect) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}

