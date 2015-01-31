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

// Generated JSON header file
#include "com_osvr_example_AnalogSync_json.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OSVR_MessageType dummyMessage;

class AnalogSyncDevice {
  public:
    AnalogSyncDevice(OSVR_PluginRegContext ctx) : m_myVal(0) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Indicate that we'll want 1 analog channel.
        osvrDeviceAnalogConfigure(opts, &m_analog, 1);

        /// Create the sync device token with the options
        osvrDeviceSyncInitWithOptions(ctx, "MySyncDevice", opts, &m_dev);

        /// Send JSON descriptor
        osvrDeviceSendJsonDescriptor(m_dev, com_osvr_example_AnalogSync_json, sizeof(com_osvr_example_AnalogSync_json));

        /// Register update callback
        osvrDeviceSyncRegisterUpdateCallback(m_dev, &AnalogSyncDevice::update,
                                             this);
    }

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

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    double m_myVal;
};

OSVR_PLUGIN(com_osvr_example_AnalogSync) {
    /// Create the device
    AnalogSyncDevice *myDevice = osvr::pluginkit::registerObjectForDeletion(
        ctx, new AnalogSyncDevice(ctx));
    return OSVR_RETURN_SUCCESS;
}
