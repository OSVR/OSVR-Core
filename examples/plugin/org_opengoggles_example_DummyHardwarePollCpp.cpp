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
#include <ogvr/PluginKit/PluginRegistration.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

class HardwarePoller {
  public:
    HardwarePoller(int initialVal) : m_data(initialVal) {}
    OGVR_ReturnCode operator()(OGVR_PluginRegContext /*ctx*/) {

        std::cout << "Got a poll for hardware change, with dummy data "
                  << m_data << std::endl;
        m_data++;
        std::cout << "Data now " << m_data << std::endl;
        return OGVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Some dummy example data.
    int m_data;
};

OGVR_PLUGIN(org_opengoggles_example_DummyHardwarePoll) {
    /// Register a polling callback function object: here passing it result of
    /// "new".
    /// Auto-registers for destruction.
    ogvr::plugin::registerHardwarePollCallback(ctx, new HardwarePoller(0));

    /// Register a polling callback function object: here passing it a value.
    /// It will make a copy that it auto-registers for destruction.
    HardwarePoller myPoller(10);
    ogvr::plugin::registerHardwarePollCallback(ctx, myPoller);
    /// Registering twice to demonstrate that copying occurs.
    ogvr::plugin::registerHardwarePollCallback(ctx, myPoller);

    return OGVR_RETURN_SUCCESS;
}
