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

#ifndef INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D
#define INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D

// Internal Includes
#include "GetVRPNConnection.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <map>
#include <string>

template <typename T> static OSVR_ReturnCode callMainloop(void *userdata) {
    T *obj = static_cast<T *>(userdata);
    obj->mainloop();
    return OSVR_RETURN_SUCCESS;
}

/// @brief Wrapper class to enforce the places where we need to have a full
/// (namespaced by plugin) name
class DeviceFullName {
  public:
    std::string const &get() const { return m_name; }

  private:
    explicit DeviceFullName(std::string const &name) : m_name(name) {}
    DeviceFullName();
    std::string m_name;
    friend class VRPNMultiserverData;
};

class VRPNMultiserverData {
  public:
    DeviceFullName
    getName(osvr::pluginhost::PluginSpecificRegistrationContext &pluginCtx,
            std::string const &nameStem);

  private:
    typedef std::map<std::string, size_t> NameCountMap;
    std::string assignName(std::string const &nameStem);
    size_t assignNumber(std::string const &nameStem);

    NameCountMap m_nameCount;
};

/// @brief Helper class that binds to the data object and a pluginregcontext and
/// provides shared functionality.
class BoundServer : boost::noncopyable {
  public:
    BoundServer(VRPNMultiserverData &data, OSVR_PluginRegContext ctx)
        : m_data(data),
          m_ctx(osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)) {
    }

    /// @brief Construct and register a device type that doesn't need
    /// anything more than a name and a connection.
    template <typename T>
    void constructAndRegister(std::string const &nameStem) {
        DeviceFullName name(getName(nameStem));
        registerDevice(name, new T(name.get().c_str(), getVRPNConnection()));
    }

    /// @brief Register a device for deletion and as an advanced device.
    /// @param name Must have come from getName!
    template <typename T>
    T *registerDevice(DeviceFullName const &name, T *dev) {
        osvr::pluginkit::registerObjectForDeletion(m_ctx.extractOpaquePointer(),
                                                   dev);

        osvr::connection::ConnectionPtr conn =
            osvr::connection::Connection::retrieveConnection(m_ctx.getParent());
        conn->registerAdvancedDevice(name.get(), &callMainloop<T>, dev);
        return dev;
    }

    /// @brief Get the full, unique (numbered) name for a new device with
    /// the given name stem.
    DeviceFullName getName(std::string const &nameStem) {
        return m_data.getName(m_ctx, nameStem);
    }

    vrpn_Connection *getVRPNConnection() { return ::getVRPNConnection(m_ctx); }

  private:
    VRPNMultiserverData &m_data;
    osvr::pluginhost::PluginSpecificRegistrationContext &m_ctx;
};

#endif // INCLUDED_VRPNMultiserver_h_GUID_05913DEF_7B12_4089_FE38_379BEF4A1A7D
