/** @file
    @brief Header forward-declaring a concealed implementation class and
    specifying the smart pointer to hold it.

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

#ifndef INCLUDED_PluginRegPtr_h_GUID_CB352900_4DBC_4A9A_B463_BABA63F80D51
#define INCLUDED_PluginRegPtr_h_GUID_CB352900_4DBC_4A9A_B463_BABA63F80D51

#include <osvr/Util/SharedPtr.h>

namespace osvr {
namespace pluginhost {
    class PluginSpecificRegistrationContextImpl;

    /// @brief Pointer with ownership semantics for cleanup of plugins.
    /// @todo why did unique_ptr not work here?
    typedef shared_ptr<PluginSpecificRegistrationContextImpl> PluginRegPtr;
} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_PluginRegPtr_h_GUID_CB352900_4DBC_4A9A_B463_BABA63F80D51
