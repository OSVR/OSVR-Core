/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Connection/Connection.h>
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/VRPNServer/GetVRPNConnection.h>
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>

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

        void registerDevice(OSVR_DeviceUpdateCallback cb, void *dev) {
            osvr::connection::ConnectionPtr conn =
                osvr::connection::Connection::retrieveConnection(
                    m_ctx.getParent());

            auto const &names = getNames();
            if (names.empty()) {
                throw std::logic_error(
                    "Your VRPN device has to register at least one name!");
            }
            m_connDev = conn->registerAdvancedDevice(names, cb, dev);
        }

        void setDeviceDescriptor(std::string const &jsonString) {
            m_connDev->setDeviceDescriptor(jsonString);
            osvr::connection::Connection::retrieveConnection(m_ctx.getParent())
                ->triggerDescriptorHandlers();
        }

        connection::ConnectionDevice::NameList const &getNames() const {
            return m_names;
        }

      private:
        pluginhost::PluginSpecificRegistrationContext &m_ctx;
        connection::ConnectionDevice::NameList m_names;
        connection::ConnectionDevicePtr m_connDev;
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

    void
    VRPNDeviceRegistration::setDeviceDescriptor(std::string const &jsonString) {
        m_impl->setDeviceDescriptor(jsonString);
    }

    void VRPNDeviceRegistration::m_registerDevice(OSVR_DeviceUpdateCallback cb,
                                                  void *dev) {
        m_impl->registerDevice(cb, dev);
    }

} // namespace vrpnserver
} // namespace osvr
