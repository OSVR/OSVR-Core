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
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginKit/Export.h>
#include <ogvr/PluginKit/MessageTypePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace ogvr {

class RegistrationContext;

class Connection;
/// @brief How one must hold a Connection.
typedef shared_ptr<Connection> ConnectionPtr;

/// @brief Class wrapping a messaging transport (server or internal) connection.
class Connection : boost::noncopyable {
  public:
    /// @name Factory methods
    ///
    /// More to come.
    /// @{
    /// @brief Factory method to create a local-machine-only connection
    OGVR_PLUGINKIT_EXPORT static ConnectionPtr createLocalConnection();
    /// @brief Factory method to create a shared connection
    OGVR_PLUGINKIT_EXPORT static ConnectionPtr createSharedConnection();
    /// @}

    /// @name Context Storage
    /// @{
    /// @brief Retrieve a connection pointer from a RegistrationContext
    OGVR_PLUGINKIT_EXPORT static ConnectionPtr
    retrieveConnection(const RegistrationContext &ctx);
    /// @brief Store a connection pointer in a RegistrationContext
    OGVR_PLUGINKIT_EXPORT static void storeConnection(RegistrationContext &ctx,
                                                      ConnectionPtr conn);
    /// @}

    /// @brief Register (or retrieve registration) of a message type.
    OGVR_PLUGINKIT_EXPORT virtual MessageTypePtr
    registerMessageType(std::string const &messageId) = 0;
    /// @brief Register a full device name. This should be namespaced with the
    /// plugin name.
    OGVR_PLUGINKIT_EXPORT virtual MessageTypePtr
    registerDevice(std::string const &deviceName) = 0;

    OGVR_PLUGINKIT_EXPORT virtual ~Connection();

  protected:
    Connection();
};
}
#endif // INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
