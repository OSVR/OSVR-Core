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
#include <osvr/Util/StringLiteralFileToString.h>

#include "com_osvr_VideoCapture_OpenCV_json.h"

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>

#include <opencv2/core/version.hpp>
#if CV_MAJOR_VERSION == 2 || CV_VERSION_EPOCH == 2
#include <opencv2/highgui/highgui.hpp> // for image capture
#else
#include <opencv2/videoio.hpp> // for image capture
#endif

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

        /// Send the JSON.
        m_dev.sendJsonDescriptor(
            osvr::util::makeString(com_osvr_VideoCapture_OpenCV_json));

        /// Sets the update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        if (!m_camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        // Get a timestamp for the upcoming camera grab.
        auto frameTime = osvr::util::time::getNow();

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

        // Send the image.
        // Note that if larger than 160x120 (RGB), will used shared memory
        // backend only.
        m_dev.send(m_imaging, osvr::pluginkit::ImagingMessage(m_frame),
                   frameTime);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    osvr::pluginkit::ImagingInterface m_imaging;
    cv::VideoCapture m_camera;
    int m_channel;
    cv::Mat m_frame;
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
    osvr::pluginkit::PluginContext context(ctx);

    context.registerHardwareDetectCallback(new CameraDetection());

    return OSVR_RETURN_SUCCESS;
}
