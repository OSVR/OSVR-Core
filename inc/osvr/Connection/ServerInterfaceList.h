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

#ifndef INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140
#define INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140

// Internal Includes
#include <osvr/Connection/BaseServerInterface.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace connection {
    typedef std::vector<ServerInterfacePtr> ServerInterfaceList;
} // namespace connection
} // namespace osvr
#endif // INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140
