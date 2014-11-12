/** @file
    @brief Implementation

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

OGVR_MessageType dummyMessage;

class DummyAsyncDevice {
  public:
    DummyAsyncDevice(OGVR_DeviceToken d) : m_dev(d) {
        std::cout << "Constructing dummy asynchronous (threaded) device"
                  << std::endl;
    }
    /// Another trampoline.
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function
    /// as long as things are running. So this function waits for the next
    /// message from the device and passes it on.
    static OGVR_ReturnCode wait(void *userData) {
        return static_cast<DummyAsyncDevice *>(userData)->m_wait();
    }
    ~DummyAsyncDevice() {
        std::cout << "Destroying dummy asynchronous (threaded) device"
                  << std::endl;
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

OGVR_PLUGIN(org_opengoggles_example_DummyAsync) {
    /// Create an asynchronous (threaded) device
    OGVR_DeviceToken d;
    ogvrDeviceAsyncInit(ctx, "MyAsyncDevice",
                        &d); // Puts an object in d that knows it's a
                             // threaded device so ogvrDeviceSendData knows
                             // that it needs to get a connection lock first.
    DummyAsyncDevice *myAsync = ogvr::pluginkit::registerObjectForDeletion(
        ctx, new DummyAsyncDevice(d));
    ogvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);
    ogvrDeviceAsyncStartWaitLoop(d, &DummyAsyncDevice::wait,
                                 static_cast<void *>(myAsync));
    return OGVR_RETURN_SUCCESS;
}
