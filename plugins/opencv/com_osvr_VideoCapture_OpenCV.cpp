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
#include <osvr/PluginKit/ImagingInterface.h>

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp> // for image capture
#include <opencv2/imgproc/imgproc.hpp> // for image scaling

#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

// Standard includes
#include <iostream>
#include <sstream>

namespace {

OSVR_MessageType cameraMessage;

class CameraDevice : boost::noncopyable {
  public:
    CameraDevice(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
        : m_camera(cameraNum), m_channel(channel) {

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Configure an imaging interface (with the default number of sensors,
        /// 1)
        m_imaging = osvr::pluginkit::ImagingInterface(opts);

        /// Come up with a device name
        std::ostringstream os;
        os << "Camera" << cameraNum << "_" << m_channel;

        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, os.str(), opts);
        // Puts an object in m_dev that knows it's a
        // threaded device so osvrDeviceSendData knows
        // that it needs to get a connection lock first.

        /// Sets the update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        if (!m_camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        // Trigger a camera grab.
        bool grabbed = m_camera.grab();
        if (!grabbed) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        bool retrieved = m_camera.retrieve(m_frame, m_channel);
        if (!retrieved) {
            return OSVR_RETURN_FAILURE;
        }
        // Currently limited to just under 64k message size.

        // cv::Mat subimage = m_frame(cv::Rect(0, 0, 210, 100));
        // m_dev.send(m_imaging, osvr::pluginkit::ImagingMessage(subimage));

        // Scale down to something under 160x120 while keeping aspect ratio.
        double xScale = 160.0 / m_frame.cols;
        double yScale = 120.0 / m_frame.rows;
        double finalScale = std::min(xScale, yScale);
        cv::resize(m_frame, m_scaledFrame, cv::Size(), finalScale, finalScale,
                   CV_INTER_AREA);
        m_dev.send(m_imaging, osvr::pluginkit::ImagingMessage(m_scaledFrame));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    osvr::pluginkit::ImagingInterface m_imaging;
    cv::VideoCapture m_camera;
    int m_channel;
    cv::Mat m_frame;
    cv::Mat m_scaledFrame;
};

class CameraDetection {
  public:
    CameraDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        {
            // Autodetect camera
            cv::VideoCapture cap(0);
            if (!cap.isOpened()) {
                // Failed to find camera
                return OSVR_RETURN_FAILURE;
            }
        }

        m_found = true;

        /// Create our device object, passing the context, and register
        /// the function to call
        osvr::pluginkit::registerObjectForDeletion(ctx, new CameraDevice(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};

} // end anonymous namespace

OSVR_PLUGIN(com_osvr_VideoCapture_OpenCV) {
    osvrDeviceRegisterMessageType(ctx, "CameraMessage", &cameraMessage);

    osvr::pluginkit::PluginContext context(ctx);

    context.registerHardwareDetectCallback(new CameraDetection());

    return OSVR_RETURN_SUCCESS;
}
