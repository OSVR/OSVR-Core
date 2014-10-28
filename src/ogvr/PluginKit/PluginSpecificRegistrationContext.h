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

// Library/third-party includes
#include <libfunctionality/PluginHandle.h>
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

namespace ogvr {
/// @brief Internal class backing the context of registrations performed by a
/// single plugin.
class PluginSpecificRegistrationContext : boost::noncopyable {
  public:
    /// @brief Constructor
    PluginSpecificRegistrationContext(std::string const &name);
    /// @brief Destructor responsible for destroying plugin data in reverse
    /// order.
    ~PluginSpecificRegistrationContext();

    /// @brief Assume ownership of the plugin handle keeping the plugin library
    /// loaded.
    void takePluginHandle(libfunc::PluginHandle &handle);

    /// @brief Call all hardware poll callbacks registered by this plugin.
    void callHardwarePollCallbacks();

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

    typedef CallbackWrapper<OGVRHardwarePollCallback> HardwarePollCallback;
    typedef std::vector<HardwarePollCallback> HardwarePollCallbackList;
    HardwarePollCallbackList m_hardwarePollCallbacks;
};
} // end of namespace ogvr

#endif // INCLUDED_PluginSpecificRegistrationContext_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
