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
#include <osvr/Connection/Connection.h>

// Library/third-party includes
#include <vrpn_Connection.h>
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace osvr {
namespace connection {
    class VrpnBasedConnection : public Connection {
      public:
        enum ConnectionType { VRPN_LOCAL_ONLY, VRPN_SHARED };

        /// @brief Constructor for the VRPN connection.
        VrpnBasedConnection(ConnectionType type);

        /// @brief Returns the vrpn_Connection pointer.
        virtual void *getUnderlyingObject();
        virtual const char *getConnectionKindID();
        virtual ~VrpnBasedConnection();

      private:
        virtual MessageTypePtr
        m_registerMessageType(std::string const &messageId);
        virtual ConnectionDevicePtr
        m_registerDevice(std::string const &deviceName);
        virtual void m_process();

        vrpn_ConnectionPtr m_vrpnConnection;
    };

} // namespace connection
} // namespace osvr
#endif // INCLUDED_VrpnBasedConnection_h_GUID_49F2C30F_D807_43B1_A754_9B645D3A1809
