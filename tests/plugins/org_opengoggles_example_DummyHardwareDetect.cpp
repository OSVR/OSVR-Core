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
#include <osvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

static int sampleHardwareUserdata = 1;

static OSVR_ReturnCode detectHardwareChange(OSVR_PluginRegContext /*ctx*/,
                                            void *userData) {
    int &data = *static_cast<int *>(userData);
    std::cout << "Got a detection request for hardware change, with user data "
              << data << std::endl;
    return OSVR_RETURN_SUCCESS;
}

OSVR_PLUGIN(org_opengoggles_example_DummyHardwareDetect) {
    /// Register a detect callback, with some dummy userdata.
    osvrPluginRegisterHardwareDetectCallback(ctx, &detectHardwareChange,
                                             &sampleHardwareUserdata);

    return OSVR_RETURN_SUCCESS;
}
