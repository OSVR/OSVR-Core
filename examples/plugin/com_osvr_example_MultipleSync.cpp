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
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

OSVR_MessageType dummyMessage;

class MultipleSyncDevice {
  public:
    MultipleSyncDevice(OSVR_DeviceToken d, OSVR_AnalogDeviceInterface analog,
                       OSVR_ButtonDeviceInterface button,
                       OSVR_TrackerDeviceInterface tracker)
        : m_dev(d), m_analog(analog), m_button(button), m_tracker(tracker),
          m_myVal(0), m_buttonPressed(false) {}

    /// Trampoline: C-compatible callback bouncing into a member function.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<MultipleSyncDevice *>(userData)->m_update();
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

        /// Toggle the button 0
        m_buttonPressed = !m_buttonPressed;
        osvrDeviceButtonSetValue(
            m_dev, m_button,
            m_buttonPressed ? OSVR_BUTTON_PRESSED : OSVR_BUTTON_NOT_PRESSED, 0);

        /// Report the identity pose for sensor 0
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        osvrDeviceTrackerSendPose(m_dev, m_tracker, &pose, 0);
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    OSVR_ButtonDeviceInterface m_button;
    OSVR_TrackerDeviceInterface m_tracker;
    double m_myVal;
    bool m_buttonPressed;
};

OSVR_PLUGIN(com_osvr_example_AnalogSync) {
    /// Create the initialization options
    OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

    /// Indicate that we'll want 1 analog channel.
    OSVR_AnalogDeviceInterface analog;
    osvrDeviceAnalogConfigure(opts, &analog, 1);

    /// Indicate that we'll want 2 buttons.
    OSVR_ButtonDeviceInterface button;
    osvrDeviceButtonConfigure(opts, &button, 2);

    /// Indicate that we'll report tracking too.
    OSVR_TrackerDeviceInterface tracker;
    osvrDeviceTrackerConfigure(opts, &tracker);

    /// Create the sync device token with the options
    OSVR_DeviceToken d;
    osvrDeviceSyncInitWithOptions(ctx, "MySyncDevice", opts, &d);
    MultipleSyncDevice *myDevice = osvr::pluginkit::registerObjectForDeletion(
        ctx, new MultipleSyncDevice(d, analog, button, tracker));

    osvrDeviceSyncRegisterUpdateCallback(d, &MultipleSyncDevice::update,
                                         myDevice);
    return OSVR_RETURN_SUCCESS;
}
