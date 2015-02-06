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

class DummyDevice {
  public:
    DummyDevice(OSVR_PluginRegContext ctx) {
        std::cout << "Constructing dummy device" << std::endl;

        /// Create an asynchronous (threaded) device
        osvrDeviceAsyncInit(
            ctx, "MyAsyncDevice",
            &m_dev); // Puts an object in m_dev that knows it's a
        // threaded device so osvrDeviceSendData knows
        // that it needs to get a connection lock first.

        /// Sets the update callback
        osvrDeviceRegisterUpdateCallback(m_dev, &DummyDevice::update, this);
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Future enhancements to the C++ wrappers will make this tiny function
    /// no longer necessary
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function as long as things are running. So this function waits for the
    /// next message from the device and passes it on.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<DummyDevice *>(userData)->m_update();
    }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
    OSVR_ReturnCode m_update() {
        // block on waiting for data.
        // once we have enough, send it.
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
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

            /// Create our device object, passing the context, and register
            /// the function to call
            osvr::pluginkit::registerObjectForDeletion(ctx,
                                                       new DummyDevice(ctx));
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(org_opengoggles_example_DummyDetectAndCreateAsync) {
    /// Register custom message type
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
