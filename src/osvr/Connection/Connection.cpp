/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    static const char CONNECTION_KEY[] = "com.osvr.ConnectionPtr";

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
    std::tuple<void *, ConnectionPtr> Connection::createLoopbackConnection() {
        auto conn = make_shared<VrpnBasedConnection>(
            VrpnBasedConnection::VRPN_LOOPBACK);
        return std::make_tuple(conn->getUnderlyingObject(),
                               ConnectionPtr{conn});
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

    ConnectionDevicePtr
    Connection::registerAdvancedDevice(std::string const &deviceName,
                                       OSVR_DeviceUpdateCallback updateFunction,
                                       void *userdata) {
        ConnectionDevicePtr dev(
            new GenericConnectionDevice(deviceName, [updateFunction, userdata] {
                return updateFunction(userdata);
            }));
        addDevice(dev);
        return dev;
    }

    ConnectionDevicePtr
    Connection::registerAdvancedDevice(NameList const &deviceNames,
                                       OSVR_DeviceUpdateCallback updateFunction,
                                       void *userdata) {
        ConnectionDevicePtr dev(new GenericConnectionDevice(
            deviceNames,
            [updateFunction, userdata] { return updateFunction(userdata); }));
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
    void Connection::registerDescriptorHandler(std::function<void()> handler) {
        m_descriptorHandlers.push_back(handler);
    }

    void Connection::triggerDescriptorHandlers() {
        for (auto &handler : m_descriptorHandlers) {
            handler();
        }
    }

    Connection::Connection() {}

    Connection::~Connection() {}

    void *Connection::getUnderlyingObject() { return nullptr; }

    const char *Connection::getConnectionKindID() { return nullptr; }

} // namespace connection
} // namespace osvr
