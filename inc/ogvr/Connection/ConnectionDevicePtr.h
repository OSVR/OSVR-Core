/** @file
    @brief Header forward-declaring ConnectionDevice and specifying the desired
   pointer to hold a ConnectionDevice in.

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

#ifndef INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
#define INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60

// Internal Includes
#include <ogvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace connection {
    class ConnectionDevice;
    /// @brief How to hold on to a ConnectionDevice
    typedef shared_ptr<ConnectionDevice> ConnectionDevicePtr;
} // namespace connection
} // namespace ogvr

#endif // INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
