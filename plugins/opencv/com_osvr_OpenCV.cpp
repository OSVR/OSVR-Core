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

// Standard includes
#include <iostream>

namespace {

OSVR_MessageType cameraMessage;

class CameraDevice {
  public:
    CameraDevice(OSVR_PluginRegContext ctx) {
        /// Create an asynchronous (threaded) device
        osvrDeviceAsyncInit(ctx, "Camera", &m_dev);
        // Puts an object in m_dev that knows it's a
        // threaded device so osvrDeviceSendData knows
        // that it needs to get a connection lock first.

        /// Sets the update callback
        osvrDeviceRegisterUpdateCallback(m_dev, &CameraDevice::update, this);
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Future enhancements to the C++ wrappers will make this tiny function
    /// no longer necessary
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function as long as things are running. So this function waits for the
    /// next message from the device and passes it on.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<CameraDevice *>(userData)->m_update();
    }

    ~CameraDevice() { }

  private:
    OSVR_ReturnCode m_update() {
        cv::VideoCapture camera(-1);
        if (!camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        // Read a frame from the camera
        cv::Mat frame;
        camera >> frame;

        // TODO Send the real frame data along
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, cameraMessage, mydata, sizeof(mydata));

        return OSVR_RETURN_SUCCESS;
    }

    OSVR_DeviceToken m_dev;
};

class CameraDetection {
  public:
    CameraDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found)
            return OSVR_RETURN_SUCCESS;

        // Autodetect camera
        cv::VideoCapture cap(-1);
        if (!cap.isOpened()) {
            // Failed to find camera
            return OSVR_RETURN_FAILURE;
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

OSVR_PLUGIN(com_osvr_example_MultipleSync) {
    osvrDeviceRegisterMessageType(ctx, "CameraMessage", &cameraMessage);

    osvr::pluginkit::PluginContext context(ctx);

    context.registerHardwareDetectCallback(new CameraDetection());

    return OSVR_RETURN_SUCCESS;
}

