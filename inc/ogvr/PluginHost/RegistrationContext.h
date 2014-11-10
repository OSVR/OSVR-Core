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
#include <ogvr/Util/AnyMap.h>
#include <ogvr/PluginHost/Export.h>
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <map>

namespace ogvr {
/// @brief PluginHost functionality: loading, hosting, registering, destroying,
/// and enumerating plugins.
namespace pluginhost {

    /// @brief Class responsible for hosting plugins, along with their
    /// registration and destruction
class RegistrationContext : boost::noncopyable {
  public:
    /// @brief basic constructor
    OGVR_PLUGINHOST_EXPORT RegistrationContext();

    /// @brief Destructor responsible for destroying plugins in reverse order.
    OGVR_PLUGINHOST_EXPORT ~RegistrationContext();

    /// @name Host-side (internal) API
    /// @{
    /// @brief Load a plugin from a dynamic library in this context
    OGVR_PLUGINHOST_EXPORT void loadPlugin(std::string const &pluginName);

    /// @brief Assume ownership of a plugin-specific registration context
    /// created and initialized outside of loadPlugin.
    OGVR_PLUGINHOST_EXPORT void
    adoptPluginRegistrationContext(PluginRegPtr ctx);

    /// @brief Trigger any registered hardware poll callbacks.
    OGVR_PLUGINHOST_EXPORT void triggerHardwarePoll();

    /// @brief Access the data storage map.
    OGVR_PLUGINHOST_EXPORT util::AnyMap &data();

    /// @brief Const access the data storage map.
    OGVR_PLUGINHOST_EXPORT util::AnyMap const &data() const;
    /// @}

  private:
    /// @brief Map of plugin names to owning pointers for plugin registration.
    typedef std::map<std::string, PluginRegPtr> PluginRegMap;

    PluginRegMap m_regMap;
    util::AnyMap m_data;
};
} // end of namespace pluginhost
} // end of namespace ogvr

#endif // INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
