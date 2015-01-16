/** @file
    @brief Header forward-declaring ConnectionDevice and specifying the desired
   pointer to hold a ConnectionDevice in.

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

#ifndef INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
#define INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    class ConnectionDevice;
    /// @brief How to hold on to a ConnectionDevice
    typedef shared_ptr<ConnectionDevice> ConnectionDevicePtr;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
