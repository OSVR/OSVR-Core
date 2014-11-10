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
#include <ogvr/Connection/Connection.h>
#include "ConnectionDevice.h"
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginHost/RegistrationContext.h>
#include <ogvr/Connection/MessageType.h>
#include "VrpnBasedConnection.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/range/algorithm.hpp>

// Standard includes
// - none

namespace ogvr {
namespace connection {
    /// @brief Internal constant string used as key into AnyMap
    static const char CONNECTION_KEY[] = "org.opengoggles.ConnectionPtr";

    ConnectionPtr Connection::createLocalConnection() {
        ConnectionPtr conn(make_shared<VrpnBasedConnection>(
            VrpnBasedConnection::VRPN_LOCAL_ONLY));
        return conn;
    }
    ConnectionPtr Connection::createSharedConnection() {
        ConnectionPtr conn(
            make_shared<VrpnBasedConnection>(VrpnBasedConnection::VRPN_SHARED));
        return conn;
    }

    ConnectionPtr
    Connection::retrieveConnection(const pluginhost::RegistrationContext &ctx) {
        ConnectionPtr ret;
        boost::any anyConn = ctx.data().get(CONNECTION_KEY);
        if (anyConn.empty()) {
            return ret;
        }
        ConnectionPtr *retPtr = boost::any_cast<ConnectionPtr>(&anyConn);
        if (retPtr) {
            ret = *retPtr;
        }
        return ret;
    }

    void Connection::storeConnection(pluginhost::RegistrationContext &ctx,
                                     ConnectionPtr conn) {
        ctx.data().set(CONNECTION_KEY, conn);
    }

    /// Wraps the derived implementation for future expandability.
    MessageTypePtr
    Connection::registerMessageType(std::string const &messageId) {
        return m_registerMessageType(messageId);
    }

    /// Wraps the derived implementation for future expandability.
    ConnectionDevicePtr
    Connection::registerDevice(std::string const &deviceName) {
        ConnectionDevicePtr dev = m_registerDevice(deviceName);
        if (dev) {
            addDevice(dev);
        }
        return dev;
    }

    void Connection::addDevice(ConnectionDevicePtr device) {
        m_devices.push_back(device);
    }

    void Connection::process() {
        // Process the connection first.
        m_process();
        // Process all devices.
        boost::for_each(m_devices,
                        [](ConnectionDevicePtr &dev) { dev->process(); });
    }

    Connection::Connection() { OGVR_DEV_VERBOSE("In Connection constructor"); }

    Connection::~Connection() { OGVR_DEV_VERBOSE("In Connection destructor"); }

    void *Connection::getUnderlyingObject() { return NULL; }

    const char *Connection::getConnectionKindID() { return NULL; }

} // namespace connection
} // namespace ogvr