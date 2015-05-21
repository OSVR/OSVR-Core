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
#include <osvr/PluginKit/EyeTrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_EyeTracker_json.h"

// Library/third-party includes

// Standard includes
#include <iostream>
#include <memory>
#include <ctime>


// Anonymous namespace to avoid symbol collision
namespace {

	//typedef std::shared_ptr<EyeTrackerInstance> TrackerPtr;

	OSVR_MessageType eyeTrackerMessage;

class EyeTrackerDevice {
  public:
	  EyeTrackerDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        //tracker = trackerInst;
		osvrDeviceEyeTrackerConfigure(opts, &m_eyetracker, 2);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "EyeTracker", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_EyeTracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

		OSVR_EyeGazeDirection leftGaze;
		OSVR_EyeGazeDirection rightGaze;
		OSVR_TimeValue times;

		osvrTimeValueGetNow(&times);

		std::srand(std::time(0));
		int randVal = std::rand();


		//Left eye Data
		OSVR_Vec2 gaze2D;
		gaze2D.data[0] = std::abs(std::sin(randVal));
		gaze2D.data[1] = std::abs(std::cos(randVal));

		OSVR_Vec3 gaze3D;
		gaze3D.data[0] = std::abs( std::sin(randVal) / (std::cos(randVal)));
		gaze3D.data[1] = std::abs( std::cos(randVal) / (std::sin(randVal)));
		gaze3D.data[2] = std::abs( std::sin(randVal) *  std::sin(randVal));

		leftGaze.gazeDirection2D = gaze2D;
		leftGaze.gazeDirection3D = gaze3D;
		//printReport(&leftGaze, 1);
		osvrDeviceEyeTrackerReportData(m_dev, m_eyetracker, leftGaze, 1, &times);

		gaze2D.data[0] = -99;
		gaze2D.data[1] = -99;

		gaze3D.data[0] = -99;
		gaze3D.data[1] = -99;
		gaze3D.data[2] = -99;

		rightGaze.gazeDirection2D = gaze2D;
		rightGaze.gazeDirection3D = gaze3D;
		//printReport(&rightGaze, 2);
		osvrDeviceEyeTrackerReportData(m_dev, m_eyetracker, rightGaze, 2, &times);
		        
        return OSVR_RETURN_SUCCESS;
    }

	void printReport(OSVR_EyeGazeDirection *report, int sensor){
		std::cout << "Sensor " << sensor << ", "
			"2D Report: " <<
			"x: " << report->gazeDirection2D.data[0] << ", " <<
			"y: " << report->gazeDirection2D.data[1] << std::endl;

		std::cout << "3D Report: " <<
			"x: " << report->gazeDirection3D.data[0] << ", " <<
			"y: " << report->gazeDirection3D.data[1] << ", " <<
			"z: " << report->gazeDirection3D.data[2] << std::endl;
	}

  private:
    osvr::pluginkit::DeviceToken m_dev;
	OSVR_EyeTrackerDeviceInterface m_eyetracker;
};

class HardwareDetection {
  public:
	  HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

		if (m_found) {
			return OSVR_RETURN_SUCCESS;
		}

		std::cout << "PLUGIN: Got a hardware detection request" << std::endl;
        
		//we always detect an eye tracker in sample plugin
		if (0 == 1){
			std::cout << "PLUGIN: We have NOT detected Eye Tracker "
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }

		m_found = true;
		
		std::cout << "PLUGIN: We have detected Eye Tracker device! " << std::endl;
		/// Create our device object
		osvr::pluginkit::registerObjectForDeletion(ctx, new EyeTrackerDevice(ctx));


        return OSVR_RETURN_SUCCESS;
    }

private:
	bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_EyeTracker) {

	osvrDeviceRegisterMessageType(ctx, "EyeTrackerMessage", &eyeTrackerMessage);

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}