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

#ifndef INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96
#define INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/BaseDevicePtr.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Factory function for a bare client device with no
    /// components/interfaces registered by default
    OSVR_COMMON_EXPORT BaseDevicePtr
    createClientDevice(std::string const &name, vrpn_ConnectionPtr const &conn);
    /// @brief Factory function for a bare server device with no
    /// components/interfaces registered by default
    OSVR_COMMON_EXPORT BaseDevicePtr
    createServerDevice(std::string const &name, vrpn_ConnectionPtr const &conn);
} // namespace common
} // namespace osvr

#endif // INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96
