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

#ifndef INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
#define INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183

// Internal Includes
#include <ogvr/Connection/Export.h>
#include <ogvr/Connection/MessageTypePtr.h>
#include <ogvr/Connection/ConnectionDevicePtr.h>
#include <ogvr/Connection/ConnectionPtr.h>
#include <ogvr/Util/DeviceCallbackTypesC.h>
#include <ogvr/PluginHost/RegistrationContext_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>
#include <functional>

namespace ogvr {
/// @brief Messaging transport and device communication functionality
/// @ingroup Connection
namespace connection {

    /// @brief Class wrapping a messaging transport (server or internal)
    /// connection.
    class Connection : boost::noncopyable {
      public:
        /// @name Factory methods
        ///
        /// More to come.
        /// @{
        /// @brief Factory method to create a local-machine-only connection
        OGVR_CONNECTION_EXPORT static ConnectionPtr createLocalConnection();
        /// @brief Factory method to create a shared connection
        OGVR_CONNECTION_EXPORT static ConnectionPtr createSharedConnection();
        /// @}

        /// @name Context Storage
        /// @{
        /// @brief Retrieve a connection pointer from a RegistrationContext
        OGVR_CONNECTION_EXPORT static ConnectionPtr
        retrieveConnection(const pluginhost::RegistrationContext &ctx);
        /// @brief Store a connection pointer in a RegistrationContext
        OGVR_CONNECTION_EXPORT static void
        storeConnection(pluginhost::RegistrationContext &ctx,
                        ConnectionPtr conn);
        /// @}

        /// @brief Register (or retrieve registration) of a message type.
        OGVR_CONNECTION_EXPORT MessageTypePtr
            registerMessageType(std::string const &messageId);

        /// @brief Register a full device name. This should be namespaced with
        /// the plugin name.
        ///
        /// This also adds the device so created to the device list.
        OGVR_CONNECTION_EXPORT ConnectionDevicePtr
            registerDevice(std::string const &deviceName);

        /// @brief Add an externally-constructed device to the device list.
        OGVR_CONNECTION_EXPORT void addDevice(ConnectionDevicePtr device);

        /// @brief Process messages. This shouldn't block.
        ///
        /// Someone needs to call this method frequently.
        OGVR_CONNECTION_EXPORT void process();

        /// @brief Destructor
        OGVR_CONNECTION_EXPORT virtual ~Connection();

        /// @name Advanced Methods - not for general consumption
        /// These can break encapsulation rules and/or encourage bad coding
        /// habits.
        /// Avoid if at all possible.
        /// @{

        /// @brief Record a full device name (namespaced with the plugin name)
        /// associated with a given callback.
        ///
        /// This does _not_ register the name in the underlying connection: it
        /// is an advanced method that assumes you have some other way of doing
        /// that.
        ///
        /// This also adds the device so created to the device list.
        OGVR_CONNECTION_EXPORT ConnectionDevicePtr
            registerAdvancedDevice(std::string const &deviceName,
                                   OGVR_SyncDeviceUpdateCallback updateFunction,
                                   void *userdata);

        /// @brief Access implementation details.
        OGVR_CONNECTION_EXPORT virtual void *getUnderlyingObject();

        /// @brief Returns some implementation-defined string based on the
        /// dynamic
        /// type of the connection.
        OGVR_CONNECTION_EXPORT virtual const char *getConnectionKindID();
        /// @}
      protected:
        /// @brief (Subclass implementation) Register (or retrieve registration)
        /// of
        /// a message type.
        virtual MessageTypePtr
        m_registerMessageType(std::string const &messageId) = 0;

        /// @brief (Subclass implementation) Register a full device name.
        virtual ConnectionDevicePtr
        m_registerDevice(std::string const &deviceName) = 0;

        /// @brief (Subclass implementation) Process messages. This shouldn't
        /// block.
        virtual void m_process() = 0;

        /// brief Constructor
        Connection();

      private:
        typedef std::vector<ConnectionDevicePtr> DeviceList;
        DeviceList m_devices;
    };
} // namespace connection
} // namespace ogvr
#endif // INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
