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
#include <osvr/Connection/Connection.h>
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Connection/MessageType.h>
#include "VrpnBasedConnection.h"
#include "GenericConnectionDevice.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/range/algorithm.hpp>
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    /// @brief Internal constant string used as key into AnyMap
    static const char CONNECTION_KEY[] = "org.opengoggles.ConnectionPtr";

    ConnectionPtr Connection::createLocalConnection() {
        ConnectionPtr conn(make_shared<VrpnBasedConnection>(
            VrpnBasedConnection::VRPN_LOCAL_ONLY));
        return conn;
    }
    ConnectionPtr Connection::createSharedConnection(
        boost::optional<std::string const &> iface, boost::optional<int> port) {
        ConnectionPtr conn(make_shared<VrpnBasedConnection>(iface, port));
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

    ConnectionDevicePtr
    Connection::createConnectionDevice(std::string const &deviceName) {
        DeviceInitObject init(shared_from_this());
        init.setName(deviceName);
        return createConnectionDevice(init);
    }

    ConnectionDevicePtr
    Connection::createConnectionDevice(DeviceInitObject &init) {
        ConnectionDevicePtr dev = m_createConnectionDevice(init);
        if (dev) {
            addDevice(dev);
        }
        return dev;
    }

    ConnectionDevicePtr Connection::registerAdvancedDevice(
        std::string const &deviceName,
        OSVR_SyncDeviceUpdateCallback updateFunction, void *userdata) {
        ConnectionDevicePtr dev(new GenericConnectionDevice(
            deviceName, std::bind(updateFunction, userdata)));
        addDevice(dev);
        return dev;
    }

    ConnectionDevicePtr Connection::registerAdvancedDevice(
        NameList const &deviceNames,
        OSVR_SyncDeviceUpdateCallback updateFunction, void *userdata) {
        ConnectionDevicePtr dev(new GenericConnectionDevice(
            deviceNames, std::bind(updateFunction, userdata)));
        addDevice(dev);
        return dev;
    }

    void Connection::addDevice(ConnectionDevicePtr device) {
        BOOST_ASSERT_MSG(device, "Device must be non-null!");
        auto const &names = device->getNames();
        if (names.size() == 1) {
            OSVR_DEV_VERBOSE("Added device: " << names.front());
        } else {
            OSVR_DEV_VERBOSE("Added device with names:");
            for (auto const &name : names) {
                OSVR_DEV_VERBOSE(" - " << name);
            }
        }
        m_devices.push_back(device);
    }

    void Connection::process() {
        // Process the connection first.
        m_process();
        // Process all devices.
        for (auto &dev : m_devices) {
            dev->process();
        }
    }

    void Connection::registerConnectionHandler(std::function<void()> handler) {
        m_registerConnectionHandler(handler);
    }

    Connection::Connection() {}

    Connection::~Connection() {}

    void *Connection::getUnderlyingObject() { return nullptr; }

    const char *Connection::getConnectionKindID() { return nullptr; }

} // namespace connection
} // namespace osvr