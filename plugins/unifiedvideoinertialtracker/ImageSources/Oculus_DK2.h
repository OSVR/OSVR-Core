/** @file
    @brief Header file describing interface for an Oculus DK2 device.

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

#ifndef INCLUDED_Oculus_DK2_h_GUID_EA07F823_BC38_47B0_F60A_E525110E2C1C
#define INCLUDED_Oculus_DK2_h_GUID_EA07F823_BC38_47B0_F60A_E525110E2C1C

// Internal Includes
// - none

// Library/third-party includes
#include <vrpn_HumanInterface.h>
#include <vrpn_Shared.h>
#include <opencv2/core/core.hpp> // for basic OpenCV types

// Standard includes
#include <vector>

namespace osvr {
namespace oculus_dk2 {

    // This is the information included with each Inertial Measurement Unit
    // (IMU) report from a DK2.
    typedef struct {
        int accel[3];
        int gyro[3];
        vrpn_uint16 timestamp;
    } OCULUS_IMU_REPORT;

    // The HID device on the DK2.  This is used to control the LEDs and
    // also to report values from the IMU.
    class Oculus_DK2_HID : public vrpn_HidInterface {
      public:
        Oculus_DK2_HID(double keepAliveSeconds =
                           9 //< How often to re-trigger the LED flashing
                       );
        virtual ~Oculus_DK2_HID();

        /// Return the latest IMU report(s).  Even if reports are not desired,
        /// must be called periodically to keep the LED patterns flashing.
        std::vector<OCULUS_IMU_REPORT> poll();

      private:
        vrpn_HidAcceptor *m_filter;
        double m_keepAliveSeconds;
        struct timeval m_lastKeepAlive;

        // Reports from the IMU.
        std::vector<OCULUS_IMU_REPORT> m_reports;

        // Send an LED control feature report. The enable flag tells
        // whether to turn on the LEDs (true) or not.
        void
        writeLEDControl(bool enable = true, vrpn_uint16 exposureLength = 350,
                        vrpn_uint16 frameInterval = 16666,
                        vrpn_uint16 vSyncOffset = 0,
                        vrpn_uint8 dutyCycle = 127 //< 255 = 100% brightness
                        ,
                        vrpn_uint8 pattern = 1, bool autoIncrement = true,
                        bool useCarrier = true, bool syncInput = false,
                        bool vSyncLock = false, bool customPattern = false,
                        vrpn_uint16 commandId = 0 //< Should always be zero
                        );

        // Send a KeepAlive feature report to the DK2.  This needs to be sent
        // every keepAliveSeconds to keep the LEDs going.
        void writeKeepAlive(
            bool keepLEDs = true //< Keep LEDs going, or only IMU?
            ,
            vrpn_uint16 interval = 10000 //< KeepAlive time in milliseconds
            ,
            vrpn_uint16 commandId = 0 //< Should always be zero
            );

        // Handle incoming data reports, which in this case are reports
        // from the Inertial Measurement Unit (IMU).
        //   TODO: Implement the actual parsing and storing of this
        // information.
        void on_data_received(size_t bytes, vrpn_uint8 *buffer);
    };

    // Function to convert a bogus-formatted image from OpenCV from the
    // HDK (which happens because the unit claims to be reporting YUV format
    // when in fact it is reporting grayscale format) into the correct
    // image.  This function returns a grayscale-format image at double
    // the resolution of the input image.
    //   TODO: The implementation of this function currently only uses half
    // of the values that are available, doubling them horizontally to fill
    // in all of the pixels.
    extern cv::Mat unscramble_image(const cv::Mat &image);

} // namespace oculus_dk2
} // namespace osvr

#endif // INCLUDED_Oculus_DK2_h_GUID_EA07F823_BC38_47B0_F60A_E525110E2C1C
