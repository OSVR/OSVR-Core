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

#ifndef INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
#define INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0

// Internal Includes
#include <ogvr/PluginKit/PluginKitC.h>
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <ogvr/Util/UniquePtr.h>
#include <ogvr/Util/CallbackWrapper.h>
#include <ogvr/Util/AnyMap.h>

// Library/third-party includes
#include <libfunctionality/PluginHandle.h>

// Standard includes
#include <vector>

namespace ogvr {
/// @brief Internal class backing the context of registrations performed by a
/// single plugin.
class PluginSpecificRegistrationContextImpl
    : public PluginSpecificRegistrationContext {
  public:
    /// @brief Constructor
    PluginSpecificRegistrationContextImpl(std::string const &name);

    /// @brief Destructor
    ///
    /// Responsible for destroying plugin data in reverse order.
    ~PluginSpecificRegistrationContextImpl();

    /// @brief Assume ownership of the plugin handle keeping the plugin library
    /// loaded.
    void takePluginHandle(libfunc::PluginHandle &handle);

    /// @brief Set parent registration context
    ///
    /// Should usually called only by RegistrationContext, and only once.
    /// If called multiple times with the same parent, this is OK.
    ///
    /// @throws std::logic_error if called when a different parent is already
    /// set.
    void setParent(RegistrationContext &parent);

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    virtual RegistrationContext &getParent();

    /// @brief Get parent registration context
    ///
    /// @throws std::logic_error if called when no parent is yet set.
    virtual RegistrationContext const &getParent() const;

    /// @brief Call all hardware poll callbacks registered by this plugin, if
    /// any.
    void callHardwarePollCallbacks();

    /// @brief Access the data storage map.
    virtual AnyMap &data();

    /// @brief Const access the data storage map.
    virtual AnyMap const &data() const;

    /// @name Plugin API
    /// @brief Called by the C API wrappers in the plugin registration headers.
    /// @{

    /// @brief Register data and a delete callback to be called on plugin
    /// unload.
    virtual void
    registerDataWithDeleteCallback(OGVR_PluginDataDeleteCallback deleteCallback,
                                   void *pluginData);

    virtual void
    registerHardwarePollCallback(OGVRHardwarePollCallback pollCallback,
                                 void *userData);
    /// @}

  private:
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

#endif // INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
