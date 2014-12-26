/** @file
    @brief Header

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

#ifndef INCLUDED_Util_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A
#define INCLUDED_Util_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace transform {
    inline double degreesToRadians(double degrees) {
        return degrees * M_PI / 180.0;
    }
} // namespace transform
} // namespace osvr
#endif // INCLUDED_Util_h_GUID_8354A4E2_30FF_429C_2569_E83EEF10E13A
