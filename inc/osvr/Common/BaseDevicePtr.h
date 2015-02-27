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

#ifndef INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6
#define INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    class BaseDevice;
    typedef shared_ptr<BaseDevice> BaseDevicePtr;
} // namespace common
} // namespace osvr

#endif // INCLUDED_BaseDevicePtr_h_GUID_F97A9FAF_5861_4C97_D38C_A223C26D61E6
