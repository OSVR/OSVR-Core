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

#ifndef INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB
#define INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace pluginhost {
    /// Get this binary's location, as a step in finding the "root" and search
    /// paths.
    std::string getBinaryLocation();
} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_BinaryLocation_h_GUID_B0594B7B_B7B8_489B_DF0D_5B3D1998D3AB
