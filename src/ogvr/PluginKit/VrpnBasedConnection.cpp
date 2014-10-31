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
#include "VrpnMessageType.h"
#include "VrpnConnectionDevice.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/range/algorithm.hpp>

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
    MessageTypePtr ret(new VrpnMessageType(messageId, m_vrpnConnection));
    return ret;
}

ConnectionDevicePtr
VrpnBasedConnection::m_registerDevice(std::string const &deviceName) {
    ConnectionDevicePtr ret =
        make_shared<VrpnConnectionDevice>(deviceName, m_vrpnConnection);
    m_devices.push_back(ret);
    return ret;
}

void VrpnBasedConnection::m_process() {
    m_vrpnConnection->mainloop();
    boost::for_each(m_devices,
                    [](ConnectionDevicePtr &dev) { dev->process(); });
}

VrpnBasedConnection::~VrpnBasedConnection() {
    /// @todo wait until all async threads are done
}
}