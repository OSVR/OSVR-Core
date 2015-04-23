/** @file
    @brief Header

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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_EyeTrackerInterface_h_GUID_D13EE1C3_EA66_4180_0C52_BBEAA16009F4
#define INCLUDED_EyeTrackerInterface_h_GUID_D13EE1C3_EA66_4180_0C52_BBEAA16009F4


// Internal Includes
#include <osvr/PluginKit/DeviceInterface.h>
#include <osvr/PluginKit/EyeTrackerInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iosfwd>

namespace osvr {
	namespace pluginkit {
		/** @defgroup PluginKitCppEyeTracker EyeTracker interface (C++)
		@brief Sending eye tracker reports from a device in your plugin.
		@ingroup PluginKit

		@{
		*/

		class EyeTrackerMessage {
		public:
			EyeTrackerMessage(OSVR_EyeGazeDirection gaze, OSVR_ChannelCount sensor = 1)
				: m_sensor(sensor), m_gaze(gaze) {}

			OSVR_EyeGazeDirection getGaze() const { return m_gaze; }

			OSVR_ChannelCount getSensor() const { return m_sensor; }

		private:
			OSVR_EyeGazeDirection m_gaze;
			OSVR_ChannelCount m_sensor;
		};

		class EyeTrackerInterface {
		public:
			EyeTrackerInterface(OSVR_EyeTrackerDeviceInterface iface = NULL)
				: m_iface(iface) {}
			explicit EyeTrackerInterface(OSVR_DeviceInitOptions opts,
				OSVR_ChannelCount numSensors = 1) {
				OSVR_ReturnCode ret =
					osvrDeviceEyeTrackerConfigure(opts, &m_iface, numSensors);
				if (OSVR_RETURN_SUCCESS != ret) {
					throw std::logic_error("Could not initialize an Eye Tracker "
						"Interface with the device options "
						"given!");
				}
			}

			void send(DeviceToken &dev, EyeTrackerMessage const &message,
				OSVR_TimeValue const &timestamp) {
				if (!m_iface) {
					throw std::logic_error(
						"Must initialize the eye tracker interface before using it!");
				}
				OSVR_EyeGazeDirection gaze(message.getGaze());
				//util::NumberTypeData typedata =
				//	util::opencvNumberTypeData(frame.type());
				//OSVR_ImagingMetadata metadata;
				//metadata.channels = frame.channels();
				//metadata.depth = typedata.getSize();
				//metadata.width = frame.cols;
				//metadata.height = frame.rows;
				//metadata.type = typedata.isFloatingPoint()
				//	? OSVR_IVT_FLOATING_POINT
				//	: (typedata.isSigned() ? OSVR_IVT_SIGNED_INT
				//	: OSVR_IVT_UNSIGNED_INT);

				OSVR_ReturnCode ret =
					osvrDeviceEyeTrackerReportData(dev, m_iface, &gaze,
					message.getSensor(), &timestamp);
				if (OSVR_RETURN_SUCCESS != ret) {
					throw std::runtime_error("Could not send imaging message!");
				}
			}

		private:
			OSVR_EyeTrackerDeviceInterface m_iface;
		};

		/// @}
	} // namespace pluginkit
}

#endif // INCLUDED_EyeTrackerInterface_h_GUID_D13EE1C3_EA66_4180_0C52_BBEAA16009F4

