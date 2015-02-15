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

#ifndef INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
#define INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A

// Internal Includes
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_ClientInterfaceObject;
namespace osvr {
namespace client {
    typedef ::OSVR_ClientInterfaceObject ClientInterface;
    /// @brief Pointer for holding ClientInterface objects safely.
    typedef shared_ptr<ClientInterface> ClientInterfacePtr;
} // namespace client
} // namespace osvr

#endif // INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
