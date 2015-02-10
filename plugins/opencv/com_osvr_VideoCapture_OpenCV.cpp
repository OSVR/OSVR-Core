/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>

// Library/third-party includes
#include <opencv2/opencv.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

// Standard includes
#include <iostream>

namespace {

OSVR_MessageType cameraMessage;

class CameraDevice : boost::noncopyable {
  public:
    CameraDevice(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
        : m_camera(cameraNum), m_channel(channel) {
        /// Create an asynchronous (threaded) device
        std::ostringstream os;
        os << "Camera" << cameraNum << "_" << m_channel;
        m_dev.initAsync(ctx, os.str());
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
        std::cout << "Grab" << std::endl;
        bool grabbed = m_camera.grab();
        if (!grabbed) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        std::cout << "Retrieve" << std::endl;
        bool retrieved = m_camera.retrieve(m_frame, m_channel);
        if (!retrieved) {
            return OSVR_RETURN_FAILURE;
        }
        std::cout << "Size:" << m_frame.size() << std::endl;
        std::cout << "Channels:" << m_frame.channels() << std::endl;
        std::cout << "Depth:" << m_frame.depth() << std::endl;

        // TODO Send the real frame data along
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, cameraMessage, mydata, sizeof(mydata));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
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
    osvrDeviceRegisterMessageType(ctx, "CameraMessage", &cameraMessage);

    osvr::pluginkit::PluginContext context(ctx);

    context.registerHardwareDetectCallback(new CameraDetection());

    return OSVR_RETURN_SUCCESS;
}
