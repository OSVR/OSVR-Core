/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>
#include <osvr/VRPNServer/GetVRPNConnection.h>
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/RegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>

namespace osvr {
namespace vrpnserver {
    class VRPNDeviceRegistration_impl : boost::noncopyable {
      public:
        VRPNDeviceRegistration_impl(
            pluginhost::PluginSpecificRegistrationContext &ctx)
            : m_ctx(ctx) {}

        pluginhost::PluginSpecificRegistrationContext &context() {
            return m_ctx;
        }

        std::string useDecoratedName(std::string const &name) {
            std::string ret = m_ctx.getName() + "/" + name;

            // Add if not already in the list.
            if (std::find(begin(m_names), end(m_names), ret) == end(m_names)) {
                m_names.push_back(ret);
            }
            return ret;
        }

        connection::ConnectionDevice::NameList const &getNames() const {
            return m_names;
        }

      private:
        pluginhost::PluginSpecificRegistrationContext &m_ctx;
        connection::ConnectionDevice::NameList m_names;
    };

    VRPNDeviceRegistration::VRPNDeviceRegistration(OSVR_PluginRegContext ctx)
        : m_ctx(pluginhost::PluginSpecificRegistrationContext::get(ctx)),
          m_impl(new VRPNDeviceRegistration_impl(m_ctx)) {}

    VRPNDeviceRegistration::VRPNDeviceRegistration(
        pluginhost::PluginSpecificRegistrationContext &context)
        : m_ctx(context), m_impl(new VRPNDeviceRegistration_impl(context)) {}

    /// Must be in .cpp file to handle pimpl deletion here.
    VRPNDeviceRegistration::~VRPNDeviceRegistration() {}

    std::string
    VRPNDeviceRegistration::useDecoratedName(std::string const &name) {
        return m_impl->useDecoratedName(name);
    }
    vrpn_Connection *VRPNDeviceRegistration::getVRPNConnection() {
        return ::osvr::vrpnserver::getVRPNConnection(m_ctx);
    }

    void VRPNDeviceRegistration::m_registerDevice(OSVR_DeviceUpdateCallback cb,
                                                  void *dev) {
        osvr::connection::ConnectionPtr conn =
            osvr::connection::Connection::retrieveConnection(m_ctx.getParent());

        auto names = m_impl->getNames();
        if (names.empty()) {
            throw std::logic_error(
                "Your VRPN device has to register at least one name!");
        }
        conn->registerAdvancedDevice(names, cb, dev);
    }

} // namespace vrpnserver
} // namespace osvr