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

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace ogvr {

class RegistrationContext;

/// @brief Class wrapping a messaging transport (server or internal) connection.
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
    retrieveConnection(const RegistrationContext &ctx);
    /// @brief Store a connection pointer in a RegistrationContext
    OGVR_CONNECTION_EXPORT static void storeConnection(RegistrationContext &ctx,
                                                       ConnectionPtr conn);
    /// @}

    /// @brief Register (or retrieve registration) of a message type.
    OGVR_CONNECTION_EXPORT MessageTypePtr
        registerMessageType(std::string const &messageId);

    /// @brief Register a full device name. This should be namespaced with the
    /// plugin name.
    OGVR_CONNECTION_EXPORT ConnectionDevicePtr
        registerDevice(std::string const &deviceName);

    /// @brief Process messages. This shouldn't block.
    ///
    /// Someone needs to call this method frequently.
    OGVR_CONNECTION_EXPORT void process();

    /// @brief Destructor
    OGVR_CONNECTION_EXPORT virtual ~Connection();

  protected:
    /// @brief (Subclass implementation) Register (or retrieve registration) of
    /// a message type.
    virtual MessageTypePtr
    m_registerMessageType(std::string const &messageId) = 0;

    /// @brief (Subclass implementation) Register a full device name.
    virtual ConnectionDevicePtr
    m_registerDevice(std::string const &deviceName) = 0;

    /// @brief (Subclass implementation) Process messages. This shouldn't block.
    virtual void m_process() = 0;

    /// brief Constructor
    Connection();
};
} // end of namespace ogvr
#endif // INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
