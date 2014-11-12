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
#include <ogvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

static int sampleHardwareUserdata = 1;

static OGVR_ReturnCode pollForHardwareChange(OGVR_PluginRegContext /*ctx*/,
                                             void *userData) {
    int &data = *static_cast<int *>(userData);
    std::cout << "Got a poll for hardware change, with user data " << data
              << std::endl;
    return OGVR_RETURN_SUCCESS;
}

OGVR_PLUGIN(org_opengoggles_example_DummyHardwarePoll) {
    /// Register a polling callback, with some dummy userdata.
    ogvrPluginRegisterHardwarePollCallback(ctx, &pollForHardwareChange,
                                           &sampleHardwareUserdata);

    return OGVR_RETURN_SUCCESS;
}
