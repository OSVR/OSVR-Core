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

#ifndef INCLUDED_CreateContext_h_GUID_F3E78707_5C69_4F80_9084_E6071A9E6128
#define INCLUDED_CreateContext_h_GUID_F3E78707_5C69_4F80_9084_E6071A9E6128

// Internal Includes
#include <osvr/Client/ClientContext_fwd.h>
#include <osvr/Client/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    OSVR_CLIENT_EXPORT ClientContext *
    createContext(const char appId[], const char host[] = "localhost");

} // namespace client
} // namespace osvr

#endif // INCLUDED_CreateContext_h_GUID_F3E78707_5C69_4F80_9084_E6071A9E6128
