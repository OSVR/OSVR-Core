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
#include <osvr/PluginKit/DeviceInterface.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>

// Generated JSON header file
#include "com_osvr_example_AnalogSync_json.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

class AnalogSyncDevice {
  public:
    AnalogSyncDevice(OSVR_PluginRegContext ctx) : m_myVal(0) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Indicate that we'll want 1 analog channel.
        osvrDeviceAnalogConfigure(opts, &m_analog, 1);

        /// Create the sync device token with the options
        m_dev.initSync(ctx, "MySyncDevice", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_example_AnalogSync_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        /// Make up some dummy data that changes to report.
        m_myVal = (m_myVal + 0.1);
        if (m_myVal > 10.0) {
            m_myVal = 0;
        }

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    double m_myVal;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_AnalogSync) {
    /// Create the device
    osvr::pluginkit::registerObjectForDeletion(ctx, new AnalogSyncDevice(ctx));
    return OSVR_RETURN_SUCCESS;
}
