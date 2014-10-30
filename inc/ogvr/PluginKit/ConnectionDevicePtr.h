/** @file
    @brief Header

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
class ConnectionDevice;
/// @brief How to hold on to a ConnectionDevice
typedef shared_ptr<ConnectionDevice> ConnectionDevicePtr;
} // end of ogvr namespace
#endif // INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
