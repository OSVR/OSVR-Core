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

#ifndef INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
#define INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A

// Internal Includes
#include <ogvr/Util/ClientOpaqueTypesC.h>
#include <ogvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OGVR_ClientInterfaceObject;
namespace ogvr {
namespace client {
    typedef ::OGVR_ClientInterfaceObject ClientInterface;
    /// @brief Pointer for holding ClientInterface objects safely.
    typedef shared_ptr<ClientInterface> ClientInterfacePtr;
} // namespace client_impl
} // namespace ogvr

#endif // INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
