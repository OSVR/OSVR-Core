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
#include <osvr/PluginKit/GestureInterfaceC.h>

// Generated JSON header file
#include "com_osvr_example_Gesture_json.h"

// Library/third-party includes

// Standard includes
#include <iostream>
#include <memory>
#include <ctime>
#include <chrono>
#include <thread>

// Anonymous namespace to avoid symbol collision
namespace {

OSVR_MessageType gestureMessage;

class GestureDevice {
  public:
    GestureDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceGestureConfigure(opts, &m_gesture, 2);
		
        /// Create the sync device token with the options
        m_dev.initSync(ctx, "Gesture", opts);

		//get an ID for gesture names to be used in plugin
		osvrDeviceGestureGetID(m_gesture, OSVR_GESTURE_DOUBLE_TAP, &m_double_tap_gesture);
		osvrDeviceGestureGetID(m_gesture, "FIST", &m_fist_gesture);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_example_Gesture_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

		std::this_thread::sleep_for(std::chrono::milliseconds(
			1000)); // Simulate waiting a quarter second for data.

        OSVR_TimeValue times;

        osvrTimeValueGetNow(&times);
		
		osvrDeviceGestureReportData(m_gesture, m_double_tap_gesture,
                                    OSVR_GESTURE_COMPLETE, 0, &times);
		osvrDeviceGestureReportData(m_gesture, m_fist_gesture,
                                    OSVR_GESTURE_COMPLETE, 1, &times);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_GestureDeviceInterface m_gesture;
	OSVR_GestureID m_fist_gesture;
	OSVR_GestureID m_double_tap_gesture;

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

        std::cout << "PLUGIN: We have detected Gesture device! " << std::endl;
        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(ctx, new GestureDevice(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_Gesture) {

    osvrDeviceRegisterMessageType(ctx, "GestureMessage", &gestureMessage);

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}