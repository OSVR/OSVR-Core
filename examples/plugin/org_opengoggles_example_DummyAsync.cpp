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

class DummyAsyncDevice {
  public:
    DummyAsyncDevice(OSVR_DeviceToken d) : m_dev(d) {
        std::cout << "Constructing dummy asynchronous (threaded) device"
                  << std::endl;
    }
    /// Another trampoline.
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function
    /// as long as things are running. So this function waits for the next
    /// message from the device and passes it on.
    static OSVR_ReturnCode wait(void *userData) {
        return static_cast<DummyAsyncDevice *>(userData)->m_wait();
    }
    ~DummyAsyncDevice() {
        std::cout << "Destroying dummy asynchronous (threaded) device"
                  << std::endl;
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

OSVR_PLUGIN(org_opengoggles_example_DummyAsync) {
    /// Create an asynchronous (threaded) device
    OSVR_DeviceToken d;
    osvrDeviceAsyncInit(ctx, "MyAsyncDevice",
                        &d); // Puts an object in d that knows it's a
                             // threaded device so osvrDeviceSendData knows
                             // that it needs to get a connection lock first.
    DummyAsyncDevice *myAsync = osvr::pluginkit::registerObjectForDeletion(
        ctx, new DummyAsyncDevice(d));
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);
    osvrDeviceAsyncStartWaitLoop(d, &DummyAsyncDevice::wait,
                                 static_cast<void *>(myAsync));
    return OSVR_RETURN_SUCCESS;
}
