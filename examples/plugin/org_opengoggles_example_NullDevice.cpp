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
#include <osvr/Util/GenericDeleter.h>

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

OSVR_PLUGIN(org_opengoggles_example_NullDevice) {

    /// Create a "device" that actually does nothing.
    DummyDevice *myDevice = new DummyDevice();
    /// Must ask the core to tell us to delete it.
    osvrPluginRegisterDataWithDeleteCallback(
        ctx, &osvr::util::generic_deleter<DummyDevice>,
        static_cast<void *>(myDevice));

    return OSVR_RETURN_SUCCESS;
}
