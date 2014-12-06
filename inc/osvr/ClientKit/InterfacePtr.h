/** @file
    @brief Header

    @date 2014

    @author
    Kevin M. Godby
    <kevin@godby.org>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_InterfacePtr_h_GUID_5A93BC25_62BC_44F2_B942_AAB6BCA01B17
#define INCLUDED_InterfacePtr_h_GUID_5A93BC25_62BC_44F2_B942_AAB6BCA01B17

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {

namespace clientkit {

    class Interface;
    typedef shared_ptr<Interface> InterfacePtr;

} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_InterfacePtr_h_GUID_5A93BC25_62BC_44F2_B942_AAB6BCA01B17
