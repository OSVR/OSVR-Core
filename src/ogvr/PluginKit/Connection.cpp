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
#include <ogvr/PluginKit/Connection.h>
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginKit/RegistrationContext.h>
#include "VrpnBasedConnection.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
/// @brief Internal constant string used as key into AnyMap
static const char CONNECTION_KEY[] = "org.opengoggles.ConnectionPtr";

ConnectionPtr Connection::createLocalConnection() {
    ConnectionPtr conn(
        make_shared<VrpnBasedConnection>(VrpnBasedConnection::VRPN_LOCAL_ONLY));
    return conn;
}
ConnectionPtr Connection::createSharedConnection() {
    ConnectionPtr conn(
        make_shared<VrpnBasedConnection>(VrpnBasedConnection::VRPN_SHARED));
    return conn;
}

ConnectionPtr Connection::retrieveConnection(const RegistrationContext &ctx) {
    ConnectionPtr ret;
    boost::any anyConn = ctx.data().get(CONNECTION_KEY);
    if (!anyConn.empty()) {
        ConnectionPtr *ptrRet = boost::any_cast<ConnectionPtr *>(anyConn);
        if (ptrRet) {
            ret = *ptrRet;
        }
    }
    return ret;
}

void Connection::storeConnection(RegistrationContext &ctx, ConnectionPtr conn) {
    ctx.data().set(CONNECTION_KEY, conn);
}

Connection::~Connection() {}
Connection::Connection() {}

} // end of namespace ogvr