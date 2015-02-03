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

        osvrDeviceSyncInit(
            ctx, "MySyncDevice",
            &m_dev); // Puts a token in m_dev that knows it's a sync
        // device so osvrDeviceSendData knows that it
        // doesn't need to acquire a lock.

        /// Sets the update callback
        osvrDeviceRegisterUpdateCallback(m_dev, &DummyDevice::update, this);
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Also something we can wrap.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<DummyDevice *>(userData)->m_update();
    }
    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
    OSVR_ReturnCode m_update() {
        std::cout << "In DummyDevice::m_update" << std::endl;
        // get some data
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
};

OSVR_PLUGIN(org_opengoggles_example_DummySync) {
    /// Register custom message type
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

    /// Create the device and register it for deletion.
    osvr::pluginkit::registerObjectForDeletion(ctx, new DummyDevice(ctx));

    return OSVR_RETURN_SUCCESS;
}
