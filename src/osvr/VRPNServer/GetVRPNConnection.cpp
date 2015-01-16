/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/VRPNServer/GetVRPNConnection.h>
#include "../Connection/VrpnConnectionKind.h" // Internal header!

// Library/third-party includes
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <vrpn_Connection.h>

// Standard includes
// - none

namespace osvr {
namespace vrpnserver {
    vrpn_Connection *getVRPNConnection(OSVR_PluginRegContext ctx) {
        return getVRPNConnection(
            osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx));
    }

    vrpn_Connection *getVRPNConnection(
        osvr::pluginhost::PluginSpecificRegistrationContext &context) {
        osvr::connection::ConnectionPtr conn =
            osvr::connection::Connection::retrieveConnection(
                context.getParent());
        vrpn_Connection *ret = nullptr;
        if (std::string(conn->getConnectionKindID()) ==
            osvr::connection::getVRPNConnectionKindID()) {
            ret = static_cast<vrpn_Connection *>(conn->getUnderlyingObject());
        }
        return ret;
    }
} // namespace vrpnserver
} // namespace osvr