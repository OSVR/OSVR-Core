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

#ifndef INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856
#define INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856

// Internal Includes
#include <ogvr/PluginHost/Export.h>
#include <ogvr/Util/PluginCallbackTypesC.h>
#include <ogvr/Util/PluginRegContextC.h>
#include <ogvr/Util/SharedPtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace ogvr {
// forward declaration
class RegistrationContext;
class PluginSpecificRegistrationContextImpl;
class AnyMap;

/// @brief Pointer with ownership semantics for cleanup of plugins.
/// @todo why did unique_ptr not work here?
typedef shared_ptr<PluginSpecificRegistrationContextImpl> PluginRegPtr;
/// @brief Class providing the external interface of a registration context
/// backing a single plugin
class PluginSpecificRegistrationContext : boost::noncopyable {
  public:
    /// @brief Factory function that creates a plugin-specific registration
    /// context.
    /// Ownership is transferred to the caller.
    ///
    /// Typically called by a RegistrationContext in the loadPlugin method, this
    /// may also be used for statically-linked "plugins" whether in deployment
    /// or testing.
    ///
    /// @param name The plugin name, conventionally in an underscore-delimited
    /// reverse DNS format.
    OGVR_PLUGINHOST_EXPORT static PluginRegPtr create(std::string const &name);

    /// @brief Retrieve this interface from an OGVR_PluginRegContext opaque
    /// pointer.
    OGVR_PLUGINHOST_EXPORT static PluginSpecificRegistrationContext &
    get(OGVR_PluginRegContext ctx);

    /// @brief Extracts the opaque pointer for this interface to send to C.
    OGVR_PLUGINHOST_EXPORT OGVR_PluginRegContext extractOpaquePointer();

    /// @brief Destructor
    OGVR_PLUGINHOST_EXPORT virtual ~PluginSpecificRegistrationContext();

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    OGVR_PLUGINHOST_EXPORT virtual RegistrationContext &getParent() = 0;

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    OGVR_PLUGINHOST_EXPORT virtual RegistrationContext const &
    getParent() const = 0;

    /// @brief Access the data storage map.
    OGVR_PLUGINHOST_EXPORT virtual AnyMap &data() = 0;

    /// @brief Const access the data storage map.
    OGVR_PLUGINHOST_EXPORT virtual AnyMap const &data() const = 0;

    /// @name Plugin API
    /// @brief Called by the C API wrappers in the plugin registration headers.
    /// @{

    /// @brief Register data and a delete callback to be called on plugin
    /// unload.
    OGVR_PLUGINHOST_EXPORT virtual void
    registerDataWithDeleteCallback(OGVR_PluginDataDeleteCallback deleteCallback,
                                   void *pluginData) = 0;

    OGVR_PLUGINHOST_EXPORT virtual void
    registerHardwarePollCallback(OGVRHardwarePollCallback pollCallback,
                                 void *userData) = 0;
    /// @}

    /// @brief Accessor for plugin name.
    OGVR_PLUGINHOST_EXPORT const std::string &getName() const;

  protected:
    /// @brief Constructor for derived class use only
    PluginSpecificRegistrationContext(std::string const &name);

  private:
    std::string const m_name;
};
} // end of namespace ogvr
#endif // INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856
