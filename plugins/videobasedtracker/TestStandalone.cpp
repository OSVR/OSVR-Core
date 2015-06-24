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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "VideoBasedTracker.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>

struct Main {

    cv::VideoCapture m_camera;
    int m_channel = 0;

    osvr::vbtracker::VideoBasedTracker m_vbtracker;

    // What type of HMD are we tracking?
    enum { Unknown, OSVRHDK, OculusDK2, Fake } m_type;

    cv::Mat m_frame;
    cv::Mat m_imageGray;

    Main() : m_camera(0) {
        int height = 0;
        int width = 0;
        if (m_camera.isOpened()) {
            height = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_HEIGHT));
            width = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_WIDTH));

            // See if this is an Oculus camera by checking the dimensions of
            // the image.  This camera type improperly describes its format
            // as being a color format when it is in fact a mono format.
            bool isOculusCamera = (width == 376) && (height == 480);
            if (isOculusCamera) {
                m_type = OculusDK2;
            }

            else {
                /// @todo Check to see if the resolution/name matches the OSVR
                /// HDK camera
                m_type = OSVRHDK;
            }
#ifdef VBHMD_DEBUG
            std::cout << "Got image of size " << width << "x" << height
                      << ", Format " << m_camera.get(CV_CAP_PROP_FORMAT)
                      << ", Mode " << m_camera.get(CV_CAP_PROP_MODE)
                      << std::endl;
            if (m_type == OculusDK2) {
                std::cout << "Is Oculus camera, reformatting to mono"
                          << std::endl;
/// @todo needed anyway!
#if 0
                m_dk2.reset(new osvr::oculus_dk2::Oculus_DK2_HID());
#endif
            }
#endif
        }

        //===============================================
        // Configure objects and set up data structures and devices based on the
        // type of device we have.

        switch (m_type) {
        case OculusDK2: {
            // TODO: Fill these in when they are known
            // m_identifiers.push_back(XXX);
            // m_estimator.push_back(XXX);
            m_vbtracker.addOculusSensor();

            // Set Oculus' camera capture parameters as described in Oliver
            // Kreylos' OculusRiftDK2VideoDevice.cpp program.  Thank you for him
            // for sharing this with us, used with permission.

            // Trying to find the closest matches to what was being done
            // in OculusRiftDK2VideoDevice.cpp, but I don't think we're going to
            // be able to set everything we need to.  In fact, these don't seem
            // to be doing anything (gain does not change the brightness, for
            // example) and all but the gain setting fails (we must not have the
            // XIMEA interface).
            /// @todo There is no OS-independent way to set these parameters on
            /// the camera, so we're not going to be able to use it.
            /// @todo Would like to set a number of things, but since these are
            ///  not working, giving up.
        } break;

        case OSVRHDK: {
            /// @todo Come up with actual estimates for camera and distortion
            /// parameters by calibrating them in OpenCV.
            double cx = width / 2.0;
            double cy = height / 2.0;
            double fx = 700.0; // XXX This needs to be in pixels, not mm
            double fy = fx;
            std::vector<std::vector<double> > m;
            m.push_back({fx, 0.0, cx});
            m.push_back({0.0, fy, cy});
            m.push_back({0.0, 0.0, 1.0});
            std::vector<double> d;
            d.push_back(0);
            d.push_back(0);
            d.push_back(0);
            d.push_back(0);
            d.push_back(0);
            m_vbtracker.addSensor(
                new osvr::vbtracker::OsvrHdkLedIdentifier(
                    osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR0_PATTERNS),
                m, d, osvr::vbtracker::OsvrHdkLedLocations_SENSOR0);
            m_vbtracker.addSensor(
                new osvr::vbtracker::OsvrHdkLedIdentifier(
                    osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                m, d, osvr::vbtracker::OsvrHdkLedLocations_SENSOR1);

        } break;

        default: // Also handles the "Unknown" case.
            // We've already got a NULL identifier and estimator, so nothing to
            // do.
            break;
        }
    }

    /// @return true if the app should exit.
    bool update() {
        if (!m_camera.isOpened()) {
            return true;
        }
        if (!m_camera.grab()) {
            // No frame available.
            return false;
        }

        if (!m_camera.retrieve(m_frame, m_channel)) {
            return true;
        }

/// @todo needed anyway!
#if 0
        //==================================================================
        // If we have an Oculus camera, then we need to reformat the
        // image pixels.
        if (m_type == OculusDK2) {
            m_imageGray = osvr::oculus_dk2::unscramble_image(m_frame);

            // Read any reports and discard them.  We do this to keep the
            // LED keepAlive going.
            m_dk2->poll();
        }
        else {
#endif
        //==================================================================
        // Convert the image into a format we can use.
        // TODO: Consider reading in the image in gray scale to begin with
        cv::cvtColor(m_frame, m_imageGray, CV_RGB2GRAY);
#if 0
        }
#endif

        return m_vbtracker.processImage(
            m_frame, m_imageGray,
            [&](OSVR_ChannelCount sensor, OSVR_Pose3 const &pose) {
                std::cout << "Sensor " << sensor << ": Translation "
                          << pose.translation << " rotation " << pose.rotation
                          << std::endl;
            });
    }
};
int main() {
    Main mainObj;
    bool done = false;
    while (!done) {
        done = mainObj.update();
    }
    return 0;
}
