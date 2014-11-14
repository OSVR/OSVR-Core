/** @file
    @brief Comprehensive example: Implementation of a dummy Hardware Detect
   Callback that creates a dummy device when it is "detected"

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/PluginKit.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

namespace {
OGVR_MessageType dummyMessage;

class DummyAsyncDevice {
  public:
    DummyAsyncDevice(OGVR_DeviceToken d) : m_dev(d) {
        std::cout << "PLUGIN: Constructing dummy device" << std::endl;
    }

    /// Future enhancements to the C++ wrappers will make this tiny function
    /// no longer necessary
    static OGVR_ReturnCode wait(void *userData) {
        return static_cast<DummyAsyncDevice *>(userData)->m_wait();
    }

    ~DummyAsyncDevice() {
        std::cout << "PLUGIN: Destroying dummy device" << std::endl;
    }

  private:
    OGVR_ReturnCode m_wait() {
        // block on waiting for data.
        // once we have enough, call
        char *mydata = NULL;
        ogvrDeviceSendData(m_dev, dummyMessage, mydata, 0);
        return OGVR_RETURN_SUCCESS;
    }
    OGVR_DeviceToken m_dev;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OGVR_ReturnCode operator()(OGVR_PluginRegContext ctx) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;
        if (!m_found) {
            std::cout << "PLUGIN: We have detected our fake device! Doing "
                         "setup stuff!" << std::endl;
            m_found = true;

            /// Our device uses a custom message type, so register that.
            ogvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

            /// Create device token
            OGVR_DeviceToken d;
            ogvrDeviceAsyncInit(ctx, "MyAsyncDevice", &d);

            /// Create our device object, passing the device token, and register
            /// the function to call
            DummyAsyncDevice *myAsync =
                ogvr::pluginkit::registerObjectForDeletion(
                    ctx, new DummyAsyncDevice(d));
            ogvrDeviceAsyncStartWaitLoop(d, &DummyAsyncDevice::wait, myAsync);
        }
        return OGVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OGVR_PLUGIN(org_opengoggles_example_DummyDetectAndCreateAsync) {
    ogvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OGVR_RETURN_SUCCESS;
}
