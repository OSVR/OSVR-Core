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

#ifndef INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809
#define INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809

// Internal Includes
#include <ogvr/PluginKit/Connection.h>

// Library/third-party includes
#include <vrpn_Connection.h>
#include <qvrpn/vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace ogvr {
class VrpnBasedConnection : public Connection {
  public:
    enum ConnectionType { VRPN_LOCAL_ONLY, VRPN_SHARED };

    /// @brief Constructor for the VRPN connection.
    VrpnBasedConnection(ConnectionType type);
    /// @brief Register (or retrieve registration) of a message type.
    virtual MessageTypePtr registerMessageType(std::string const &messageId);
    /// @brief Register a full device name. This should be namespaced with the
    /// plugin name.
    virtual MessageTypePtr registerDevice(std::string const &deviceName);

    virtual ~VrpnBasedConnection();

  private:
    vrpn_ConnectionPtr m_vrpnConnection;
};

} // end of namespace ogvr
#endif // INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809
