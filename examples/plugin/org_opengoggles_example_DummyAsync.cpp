/** @file
    @brief Implementation

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
        osvrDeviceRegisterUpdateCallback(m_dev, &DummyDevice::wait, this);
    }
    /// Another trampoline.
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function as long as things are running. So this function waits for the
    /// next message from the device and passes it on.
    static OSVR_ReturnCode wait(void *userData) {
        return static_cast<DummyDevice *>(userData)->m_wait();
    }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
    OSVR_ReturnCode m_wait() {
        // block on waiting for data.
        // once we have enough, send it.
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
};

OSVR_PLUGIN(org_opengoggles_example_DummyAsync) {
    /// Register custom message type
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

    /// Create the device and register it for deletion.
    osvr::pluginkit::registerObjectForDeletion(ctx, new DummyDevice(ctx));

    return OSVR_RETURN_SUCCESS;
}
