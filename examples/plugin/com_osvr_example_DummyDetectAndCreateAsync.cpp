/** @file
    @brief Comprehensive example: Implementation of a dummy Hardware Detect
   Callback that creates a dummy device when it is "detected"

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
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

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {
OSVR_MessageType dummyMessage;

class DummyDevice {
  public:
    DummyDevice(OSVR_PluginRegContext ctx) {
        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, "MyAsyncDevice");

        /// Sets the update callback
        m_dev.registerUpdateCallback(this);
    }

    /// In this case, the core spawns a thread, with a loop calling this
    /// function as long as things are running. So this function waits for the
    /// next message from the device and passes it on.
    OSVR_ReturnCode update() {
        // block on waiting for data.
        // once we have enough, send it.
        const char mydata[] = "something";
        m_dev.sendData(dummyMessage, mydata);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;
        if (!m_found) {
            std::cout << "PLUGIN: We have detected our fake device! Doing "
                         "setup stuff!" << std::endl;
            m_found = true;

            /// Create our device object, passing the context
            osvr::pluginkit::registerObjectForDeletion(ctx,
                                                       new DummyDevice(ctx));
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_DummyDetectAndCreateAsync) {
    /// Register custom message type
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
