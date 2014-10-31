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

#ifndef INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
#define INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387

// Internal Includes
#include <ogvr/Util/UniquePtr.h>
#include <ogvr/PluginKit/ConnectionPtr.h>
#include <ogvr/PluginKit/ConnectionDevicePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace ogvr {
class MessageType;
class PluginSpecificRegistrationContext;
class AsyncDeviceToken;
class SyncDeviceToken;
class DeviceToken;
typedef unique_ptr<DeviceToken> DeviceTokenPtr;

class DeviceToken {
  public:
    /// @name Factory functions
    /// @{
    static DeviceTokenPtr createAsyncDevice(std::string const &name,
                                            ConnectionPtr const &conn);
    static DeviceTokenPtr createSyncDevice(std::string const &name,
                                           ConnectionPtr const &conn);
    /// @}

    /// @brief Destructor
    virtual ~DeviceToken();

    /// @brief "Casting" function - returns a valid pointer if and only if this
    /// is an AsyncDeviceToken
    virtual AsyncDeviceToken *asAsyncDevice();

    /// @brief "Casting" function - returns a valid pointer if and only if this
    /// is a SyncDeviceToken
    virtual SyncDeviceToken *asSyncDevice();

    /// @brief Accessor for name property
    std::string const &getName() const;

    /// @brief Send data
    void sendData(MessageType *type, const char *bytestream, size_t len);

  protected:
    DeviceToken(std::string const &name);
    ConnectionPtr m_getConnection();
    ConnectionDevicePtr m_getConnectionDevice();
    virtual void m_sendData(MessageType *type, const char *bytestream,
                            size_t len) = 0;

  private:
    void m_sharedInit(ConnectionPtr const &conn);
    std::string const m_name;
    ConnectionPtr m_conn;
    ConnectionDevicePtr m_dev;
};
} // end of namespace ogvr

#endif // INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
