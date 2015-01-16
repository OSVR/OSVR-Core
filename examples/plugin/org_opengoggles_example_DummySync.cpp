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

class DummySyncDevice {
  public:
    DummySyncDevice(OSVR_DeviceToken d) : m_dev(d) {
        std::cout << "Constructing dummy synchronous device" << std::endl;
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Also something we can wrap.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<DummySyncDevice *>(userData)->m_update();
    }
    ~DummySyncDevice() {
        std::cout << "Destroying dummy synchronous device" << std::endl;
    }

  private:
    OSVR_ReturnCode m_update() {
        std::cout << "In DummySyncDevice::m_update" << std::endl;
        // get some data
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
};

OSVR_PLUGIN(org_opengoggles_example_DummySync) {
    /// Create a synchronous (in the mainloop) device
    OSVR_DeviceToken d;
    osvrDeviceSyncInit(ctx, "MySyncDevice",
                       &d); // Puts a token in d that knows it's a sync
                            // device so osvrDeviceSendData knows that it
                            // doesn't need to acquire a lock.
    DummySyncDevice *mySync =
        osvr::pluginkit::registerObjectForDeletion(ctx, new DummySyncDevice(d));

    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);
    osvrDeviceSyncRegisterUpdateCallback(d, &DummySyncDevice::update,
                                         static_cast<void *>(mySync));
    return OSVR_RETURN_SUCCESS;
}
