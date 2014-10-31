/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/PluginInterfaceC.h>
#include <ogvr/PluginKit/PluginRegistration.h>
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OGVR_MessageType dummyMessage;

class DummySyncDevice {
  public:
    DummySyncDevice(OGVR_DeviceToken d) : m_dev(d) {
        std::cout << "Constructing dummy synchronous device" << std::endl;
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Also something we can wrap.
    static OGVR_PluginReturnCode update(void *userData) {
        return static_cast<DummySyncDevice *>(userData)->m_update();
    }
    ~DummySyncDevice() {
        std::cout << "Destroying dummy synchronous device" << std::endl;
    }

  private:
    OGVR_PluginReturnCode m_update() {
        std::cout << "In DummySyncDevice::m_update" << std::endl;
        // get some data
        const char mydata[] = "something";
        ogvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
        return OGVR_PLUGIN_SUCCESS;
    }
    OGVR_DeviceToken m_dev;
};

OGVR_PLUGIN(org_opengoggles_example_DummySync) {
    /// Create a synchronous (in the mainloop) device
    OGVR_DeviceToken d;
    ogvrDeviceSyncInit(ctx, "MySyncDevice",
                       &d); // Puts a token in d that knows it's a sync
                            // device so ogvrDeviceSendData knows that it
                            // doesn't need to acquire a lock.
    DummySyncDevice *mySync =
        ogvr::plugin::registerObjectForDeletion(ctx, new DummySyncDevice(d));

    ogvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);
    ogvrDeviceSyncRegisterUpdateCallback(d, &DummySyncDevice::update,
                                         static_cast<void *>(mySync));
    return OGVR_PLUGIN_SUCCESS;
}
