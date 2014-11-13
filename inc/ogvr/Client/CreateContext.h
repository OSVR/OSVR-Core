/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
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
#include <ogvr/Client/ClientContext_fwd.h>
#include <ogvr/Client/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace client {

    OGVR_CLIENT_EXPORT ClientContext *
    createContext(const char appId[], const char host[] = "localhost");

} // namespace client
} // namespace ogvr

#endif // INCLUDED_CreateContext_h_GUID_F3E78707_5C69_4F80_9084_E6071A9E6128
