/** @file
    @brief Header

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

#ifndef INCLUDED_VRPNDeviceServer_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2
#define INCLUDED_VRPNDeviceServer_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2

// Internal Includes
#include <osvr/VRPNServer/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/PluginRegContextC.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
// - none

class vrpn_Connection;

namespace osvr {
namespace vrpnserver {
    class VRPNDeviceRegistration_impl;

    namespace detail {
        template <typename T>
        static OSVR_ReturnCode callMainloop(void *userdata) {
            T *obj = static_cast<T *>(userdata);
            obj->mainloop();
            return OSVR_RETURN_SUCCESS;
        }
    } // namespace detail

    class VRPNDeviceRegistration : boost::noncopyable {
      public:
        /// @brief Start the process of registering a manually-created VRPN
        /// device into the OSVR server core.
        VRPNDeviceRegistration(OSVR_PluginRegContext ctx);
        /// @overload
        VRPNDeviceRegistration(
            pluginhost::PluginSpecificRegistrationContext &context);
        /// @brief destructor
        ~VRPNDeviceRegistration();

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
        std::string useDecoratedName(std::string const &name);

        /// @brief Get the vrpn_Connection object to use in constructing your
        /// object.
        vrpn_Connection *getVRPNConnection();

        /// @brief Registers your custom device with the server and takes
        /// ownership of the object.
        template <typename T> T *registerDevice(T *dev) {
            osvr::pluginkit::registerObjectForDeletion(
                m_ctx.extractOpaquePointer(), dev);
            m_registerDevice(&detail::callMainloop<T>,
                             static_cast<void *>(dev));
            return dev;
        }

      private:
        void m_registerDevice(OSVR_SyncDeviceUpdateCallback cb, void *dev);
        pluginhost::PluginSpecificRegistrationContext &m_ctx;
        unique_ptr<VRPNDeviceRegistration_impl> m_impl;
    };
} // namespace vrpnserver
} // namespace osvr

#endif // INCLUDED_VRPNDeviceServer_h_GUID_1D69B168_9B9C_47B9_A9F3_9A569E9710D2
