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

#ifndef INCLUDED_CreatePluginSpecificRegistrationContext_h_GUID_8C9A89AA_4D67_49AD_A21E_1B0884C7990E
#define INCLUDED_CreatePluginSpecificRegistrationContext_h_GUID_8C9A89AA_4D67_49AD_A21E_1B0884C7990E

// Internal Includes
#include <ogvr/PluginKit/Export.h>
#include <ogvr/PluginKit/PluginRegistrationC.h>
#include <ogvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace ogvr {
class PluginSpecificRegistrationContext;
/// @brief Pointer with ownership semantics for cleanup of plugins.
/// @todo why did unique_ptr not work here?
typedef shared_ptr<PluginSpecificRegistrationContext> PluginRegPtr;

/// @brief Factory function that creates a plugin-specific registration context.
/// Ownership is transferred to the caller.
///
/// Typically called by a RegistrationContext in the loadPlugin method, this
/// may also be used for statically-linked "plugins" whether in deployment
/// or testing.
///
/// @param name The plugin name, conventionally in an underscore-delimited
/// reverse DNS format.
OGVR_PLUGINKIT_EXPORT PluginRegPtr
    createPluginSpecificRegistrationContext(std::string const &name);

/// @brief Extracts the plugin registration context from a PluginRegPtr so it
/// can be passed across
/// the C interface to a plugin's entry point function.
///
/// No ownership is transferred.
OGVR_PLUGINKIT_EXPORT OGVR_PluginRegContext
    extractPluginRegistrationContext(PluginRegPtr const &ctx);

} // end of namespace ogvr

#endif // INCLUDED_CreatePluginSpecificRegistrationContex_h_GUID_8C9A89AA_4D67_49AD_A21E_1B0884C7990E
