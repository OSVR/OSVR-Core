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

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>

// Anonymous namespace to avoid symbol collision
namespace {

class MultipleAsyncDevice {
  public:
    MultipleAsyncDevice(OSVR_PluginRegContext ctx)
        : m_myVal(0), m_buttonPressed(false) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Indicate that we'll want 1 analog channel.
        osvrDeviceAnalogConfigure(opts, &m_analog, 1);

        /// Indicate that we'll want 1 button.
        osvrDeviceButtonConfigure(opts, &m_button, 1);

        /// Indicate that we'll report tracking too.
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the async device token with the options
        m_dev.initSync(ctx, "MyAsyncDevice", opts);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        /// Because this is an async device, we can block for data,
        /// simulated here with a sleep.
        std::this_thread::sleep_for(std::chrono::seconds(1));

        /// Make up some dummy data that changes to report.
        m_myVal = (m_myVal + 0.1);
        if (m_myVal > 10.0) {
            m_myVal = 0;
        }

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);

        /// Toggle the button 0
        m_buttonPressed = !m_buttonPressed;
        osvrDeviceButtonSetValue(
            m_dev, m_button,
            m_buttonPressed ? OSVR_BUTTON_PRESSED : OSVR_BUTTON_NOT_PRESSED, 0);

        /// Report the identity pose for sensor 0
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        osvrDeviceTrackerSendPose(m_dev, m_tracker, &pose, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    OSVR_ButtonDeviceInterface m_button;
    OSVR_TrackerDeviceInterface m_tracker;
    double m_myVal;
    bool m_buttonPressed;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_MultipleAsync) {

    /// Create device object.
    osvr::pluginkit::registerObjectForDeletion(ctx,
                                               new MultipleAsyncDevice(ctx));

    return OSVR_RETURN_SUCCESS;
}
