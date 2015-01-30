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
#include <osvr/PluginKit/AnalogInterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OSVR_MessageType dummyMessage;

class AnalogSyncDevice {
  public:
    AnalogSyncDevice(OSVR_DeviceToken d, OSVR_AnalogDeviceInterface analog)
        : m_dev(d), m_analog(analog), m_myVal(0) {}

    /// Trampoline: C-compatible callback bouncing into a member function.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<AnalogSyncDevice *>(userData)->m_update();
    }

  private:
    OSVR_ReturnCode m_update() {
        /// Make up some dummy data that changes to report.
        m_myVal = (m_myVal + 0.1);
        if (m_myVal > 10.0) {
            m_myVal = 0;
        }
        std::cout << "Reporting value " << m_myVal << std::endl;

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    double m_myVal;
};

OSVR_PLUGIN(com_osvr_example_AnalogSync) {
    /// Create the initialization options
    OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

    /// Indicate that we'll want 1 analog channel.
    OSVR_AnalogDeviceInterface analog;
    osvrDeviceAnalogConfigure(opts, &analog, 1);

    /// Create the sync device token with the options
    OSVR_DeviceToken d;
    osvrDeviceSyncInitWithOptions(ctx, "MySyncDevice", opts, &d);
    AnalogSyncDevice *myDevice = osvr::pluginkit::registerObjectForDeletion(
        ctx, new AnalogSyncDevice(d, analog));

    osvrDeviceSyncRegisterUpdateCallback(d, &AnalogSyncDevice::update,
                                         myDevice);
    return OSVR_RETURN_SUCCESS;
}
