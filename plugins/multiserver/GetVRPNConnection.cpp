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
#include "GetVRPNConnection.h"

// Library/third-party includes
#include <ogvr/Connection/Connection.h>
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <vrpn_Connection.h>

// Standard includes
// - none

// Sneaky way to get an ID for VRPN.
/// @todo solve this problem in a more sophisticated way.
OGVR_CONNECTION_EXPORT const char *getVRPNConnectionKindID();

vrpn_Connection *getVRPNConnection(OGVR_PluginRegContext ctx) {
    ogvr::ConnectionPtr conn = ogvr::Connection::retrieveConnection(
        ogvr::PluginSpecificRegistrationContext::get(ctx).getParent());
    vrpn_Connection *ret = NULL;
    if (std::string(conn->getConnectionKindID()) == getVRPNConnectionKindID()) {
        ret = static_cast<vrpn_Connection *>(conn->getUnderlyingObject());
    }
    return ret;
}