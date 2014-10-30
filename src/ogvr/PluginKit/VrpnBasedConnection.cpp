/** @file
    @brief Implementation

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

// Internal Includes
#include "VrpnBasedConnection.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
VrpnBasedConnection::VrpnBasedConnection(ConnectionType type) {
    switch (type) {
    case VRPN_LOCAL_ONLY: {
        m_vrpnConnection =
            vrpn_ConnectionPtr::create_server_connection("127.0.0.1");
    }
    case VRPN_SHARED: {
        m_vrpnConnection = vrpn_ConnectionPtr::create_server_connection();
    }
    }
}

MessageTypePtr
VrpnBasedConnection::m_registerMessageType(std::string const &messageId) {
    ///@todo implement
    return MessageTypePtr();
}

ConnectionDevicePtr
VrpnBasedConnection::m_registerDevice(std::string const &deviceName) {
    ///@todo implement
    return ConnectionDevicePtr();
}

VrpnBasedConnection::~VrpnBasedConnection() {}
}