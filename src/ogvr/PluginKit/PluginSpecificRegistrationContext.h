/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_PluginSpecificRegistrationContext_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
#define INCLUDED_PluginSpecificRegistrationContext_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0

// Internal Includes
#include <ogvr/PluginKit/PluginInterfaceC.h>
#include <ogvr/Util/UniquePtr.h>
#include <ogvr/Util/CallbackWrapper.h>
#include <ogvr/Util/AnyMap.h>

// Library/third-party includes
#include <libfunctionality/PluginHandle.h>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

namespace ogvr {
// forward declaration
class RegistrationContext;

/// @brief Internal class backing the context of registrations performed by a
/// single plugin.
class PluginSpecificRegistrationContext : boost::noncopyable {
  public:
    /// @brief Constructor
    PluginSpecificRegistrationContext(std::string const &name);

    /// @brief Destructor
    ///
    /// Responsible for destroying plugin data in reverse order.
    ~PluginSpecificRegistrationContext();

    /// @brief Assume ownership of the plugin handle keeping the plugin library
    /// loaded.
    void takePluginHandle(libfunc::PluginHandle &handle);

    /// @brief Set parent registration context
    ///
    /// Should be called only by RegistrationContext, and only once.
    /// @throws std::logic_error if called when a parent is already set.
    void setParent(RegistrationContext &parent);

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    RegistrationContext &getParent();

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    RegistrationContext const &getParent() const;

    /// @brief Call all hardware poll callbacks registered by this plugin, if
    /// any.
    void callHardwarePollCallbacks();

    /// @brief Accessor for plugin name.
    const std::string &getName() const;

    /// @brief Access the data storage map.
    AnyMap &data();

    /// @brief Const access the data storage map.
    AnyMap const &data() const;

    /// @name Plugin API
    /// @brief Called by the C API wrappers in the plugin registration headers.
    /// @{

    /// @brief Register data and a delete callback to be called on plugin
    /// unload.
    void
    registerDataWithDeleteCallback(OGVR_PluginDataDeleteCallback deleteCallback,
                                   void *pluginData);

    void registerHardwarePollCallback(OGVRHardwarePollCallback pollCallback,
                                      void *userData);
    /// @}

  private:
    std::string const m_name;
    /// @brief Pointer with ownership semantics for deletion of plugin data.
    typedef unique_ptr<void, OGVR_PluginDataDeleteCallback> PluginDataPtr;
    /// @brief List of plugin data.
    typedef std::vector<PluginDataPtr> PluginDataList;

    PluginDataList m_dataList;
    libfunc::PluginHandle m_handle;
    RegistrationContext *m_parent;

    typedef CallbackWrapper<OGVRHardwarePollCallback> HardwarePollCallback;
    typedef std::vector<HardwarePollCallback> HardwarePollCallbackList;
    HardwarePollCallbackList m_hardwarePollCallbacks;

    AnyMap m_data;
};
} // end of namespace ogvr

#endif // INCLUDED_PluginSpecificRegistrationContext_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
