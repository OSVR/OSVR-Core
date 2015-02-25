/** @file
    @brief Header

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

#ifndef INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB
#define INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace common {
    class DeviceComponent;
    typedef shared_ptr<DeviceComponent> DeviceComponentPtr;
    typedef std::vector<DeviceComponentPtr> DeviceComponentList;
} // namespace common
} // namespace osvr

#endif // INCLUDED_DeviceComponentPtr_h_GUID_D2258125_987B_42A9_7215_3EED5B9622BB
