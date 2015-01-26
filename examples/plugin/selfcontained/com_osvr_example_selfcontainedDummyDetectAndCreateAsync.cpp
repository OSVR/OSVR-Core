/** @file
    @brief Comprehensive example: Implementation of a dummy Hardware Detect
   Callback that creates a dummy device when it is "detected"

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

namespace {
OSVR_MessageType dummyMessage;

class DummyAsyncDevice {
  public:
    DummyAsyncDevice(OSVR_DeviceToken d) : m_dev(d) {
        std::cout << "PLUGIN: Constructing dummy device" << std::endl;
    }

    /// Future enhancements to the C++ wrappers will make this tiny function
    /// no longer necessary
    static OSVR_ReturnCode wait(void *userData) {
        return static_cast<DummyAsyncDevice *>(userData)->m_wait();
    }

    ~DummyAsyncDevice() {
        std::cout << "PLUGIN: Destroying dummy device" << std::endl;
    }

  private:
    OSVR_ReturnCode m_wait() {
        // block on waiting for data.
        // once we have enough, call
        char *mydata = NULL;
        osvrDeviceSendData(m_dev, dummyMessage, mydata, 0);
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;
        if (!m_found) {
            std::cout << "PLUGIN: We have detected our fake device! Doing "
                         "setup stuff!" << std::endl;
            m_found = true;

            /// Our device uses a custom message type, so register that.
            osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

            /// Create device token
            OSVR_DeviceToken d;
            osvrDeviceAsyncInit(ctx, "MyAsyncDevice", &d);

            /// Create our device object, passing the device token, and register
            /// the function to call
            DummyAsyncDevice *myAsync =
                osvr::pluginkit::registerObjectForDeletion(
                    ctx, new DummyAsyncDevice(d));
            osvrDeviceAsyncStartWaitLoop(d, &DummyAsyncDevice::wait, myAsync);
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_selfcontainedDummyDetectAndCreateAsync) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
