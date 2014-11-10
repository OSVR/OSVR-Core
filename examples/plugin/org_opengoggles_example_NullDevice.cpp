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
#include <ogvr/PluginKit/PluginRegistrationC.h>
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

class DummyDevice {
  public:
    DummyDevice() { std::cout << "Constructing dummy device" << std::endl; }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
};

OGVR_PLUGIN(org_opengoggles_example_NullDevice) {

    /// Create a "device" that actually does nothing.
    DummyDevice *myDevice = new DummyDevice();
    /// Must ask the core to tell us to delete it.
    ogvrPluginRegisterDataWithDeleteCallback(
        ctx, &ogvr::util::generic_deleter<DummyDevice>,
        static_cast<void *>(myDevice));

    return OGVR_RETURN_SUCCESS;
}
