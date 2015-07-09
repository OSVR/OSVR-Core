/** @file
    @brief Header

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

#ifndef INCLUDED_VRPNDeviceRegistration_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2
#define INCLUDED_VRPNDeviceRegistration_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2

// Internal Includes
#include <osvr/VRPNServer/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/PluginKit/PluginRegistration.h>
#include <osvr/Util/PluginRegContextC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

class vrpn_Connection;

namespace osvr {
namespace vrpnserver {
    class VRPNDeviceRegistration_impl;

    namespace detail {
        /// @brief This function is the one that will serve as your "update
        /// callback" for your VRPN-based device.
        template <typename T>
        static OSVR_ReturnCode callMainloop(void *userdata) {
            T *obj = static_cast<T *>(userdata);
            obj->mainloop();
            return OSVR_RETURN_SUCCESS;
        }
    } // namespace detail

    class VRPNDeviceRegistration {
      public:
        /// @brief noncopyable
        VRPNDeviceRegistration(VRPNDeviceRegistration const &) = delete;
        /// @brief nonassignable
        VRPNDeviceRegistration &
        operator=(VRPNDeviceRegistration const &) = delete;
        /// @brief Start the process of registering a manually-created VRPN
        /// device into the OSVR server core.
        OSVR_VRPNSERVER_EXPORT
        VRPNDeviceRegistration(OSVR_PluginRegContext ctx);
        /// @overload
        OSVR_VRPNSERVER_EXPORT VRPNDeviceRegistration(
            pluginhost::PluginSpecificRegistrationContext &context);
        /// @brief destructor
        OSVR_VRPNSERVER_EXPORT ~VRPNDeviceRegistration();

        /// @brief Decorates a device name with the plugin name and returns it,
        /// as well as records it for registration with the connection.
        ///
        /// You must make at least one call to this, to get a proper name for
        /// your device.
        ///
        /// If you make more calls, then the first call should be for the
        /// "primary" device. Note that order of evaluation of arguments (for
        /// instance, to a constructor call) is undefined, so you'll want to
        /// call this first and store its result in a temporary string before
        /// your constructor call if you need more than one name.
        OSVR_VRPNSERVER_EXPORT std::string
        useDecoratedName(std::string const &name);

        /// @brief Get the vrpn_Connection object to use in constructing your
        /// object.
        OSVR_VRPNSERVER_EXPORT vrpn_Connection *getVRPNConnection();

        /// @brief Registers your custom device with the server and takes
        /// ownership of the object.
        ///
        /// @returns the pointer you passed in, to make it easy to use this
        /// method to wrap a call to new.
        template <typename T> T *registerDevice(T *dev) {
            osvr::pluginkit::registerObjectForDeletion(
                m_ctx.extractOpaquePointer(), dev);
            m_registerDevice(&detail::callMainloop<T>,
                             static_cast<void *>(dev));
            return dev;
        }

        /// @brief Constructs and registers your custom device with the server
        /// and takes ownership of the object. Requires that your device only
        /// need a name and a vrpn_Connection to construct.
        ///
        /// This is a shortcut method that handles calling useDecoratedName()
        /// and registerDevice() for you for the trivial case.
        ///
        /// @param name Un-decorated name for your device - the function will
        /// take care of decorating it for you.
        /// @returns A pointer to your device, in case post-construction
        /// configuration is required.
        template <typename T>
        T *constructAndRegisterDevice(std::string const &name) {
            unique_ptr<T> ret(
                new T(useDecoratedName(name).c_str(), getVRPNConnection()));
            return registerDevice(ret.release());
        }

        OSVR_VRPNSERVER_EXPORT void
        setDeviceDescriptor(std::string const &jsonString);

      private:
        OSVR_VRPNSERVER_EXPORT void
        m_registerDevice(OSVR_DeviceUpdateCallback cb, void *dev);
        pluginhost::PluginSpecificRegistrationContext &m_ctx;
        unique_ptr<VRPNDeviceRegistration_impl> m_impl;
    };
} // namespace vrpnserver
} // namespace osvr

#endif // INCLUDED_VRPNDeviceRegistration_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2
