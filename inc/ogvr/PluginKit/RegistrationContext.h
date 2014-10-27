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

#ifndef INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
#define INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10

// Internal Includes
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginKit/Export.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <map>

namespace ogvr {
class PluginSpecificRegistrationContext;
/// @brief Internal class responsible for the registration and destruction of
/// plugins.
class RegistrationContext : boost::noncopyable {
  public:
    /// @brief basic constructor
    OGVR_PLUGINKIT_EXPORT RegistrationContext();

    /// @brief Destructor responsible for destroying plugins in reverse order.
    OGVR_PLUGINKIT_EXPORT ~RegistrationContext();

    /// @name Host-side (internal) API
    /// @{
    /// @brief load a plugin in this context
    OGVR_PLUGINKIT_EXPORT void loadPlugin(std::string const &pluginName);

    /// @}

  private:
    /// @brief Pointer with ownership semantics for cleanup of plugins.
    /// @todo why did unique_ptr not work here?
    typedef shared_ptr<PluginSpecificRegistrationContext> PluginRegPtr;
    /// @brief Map of plugin names to owning pointers for plugin registration.
    typedef std::map<std::string, PluginRegPtr> PluginRegMap;

    PluginRegMap m_regMap;
};
} // end of namespace ogvr

#endif // INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
