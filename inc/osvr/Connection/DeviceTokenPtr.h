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

#ifndef INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A
#define INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A

// Internal Includes
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_DeviceTokenObject;
namespace osvr {
namespace connection {
    typedef OSVR_DeviceTokenObject DeviceToken;
    typedef unique_ptr<DeviceToken> DeviceTokenPtr;
    class AsyncDeviceToken;
    class SyncDeviceToken;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceTokenPtr_h_GUID_0B9B2AF5_09FE_4F8C_FDEC_98BE876F020A
