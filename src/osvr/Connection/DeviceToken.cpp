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
#include <osvr/Connection/DeviceToken.h>
#include "AsyncDeviceToken.h"
#include "SyncDeviceToken.h"
#include "VirtualDeviceToken.h"
#include <osvr/Connection/Connection.h>
#include <osvr/Connection/ConnectionDevice.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace osvr {
namespace connection {

    DeviceTokenPtr DeviceToken::createAsyncDevice(DeviceInitObject &init) {
        DeviceTokenPtr ret(new AsyncDeviceToken(init.getQualifiedName()));
        ret->m_sharedInit(init.getConnection());
        return ret;
    }

    DeviceTokenPtr DeviceToken::createSyncDevice(DeviceInitObject &init) {
        DeviceTokenPtr ret(new AsyncDeviceToken(init.getQualifiedName()));
        ret->m_sharedInit(init.getConnection());
        return ret;
    }

    DeviceTokenPtr DeviceToken::createVirtualDevice(std::string const &name,
                                                    ConnectionPtr const &conn) {
        DeviceTokenPtr ret(new VirtualDeviceToken(name));
        ret->m_sharedInit(conn);
        return ret;
    }

    DeviceToken::DeviceToken(std::string const &name) : m_name(name) {}

    DeviceToken::~DeviceToken() {}

    std::string const &DeviceToken::getName() const { return m_name; }

    void DeviceToken::sendData(MessageType *type, const char *bytestream,
                               size_t len) {
        util::time::TimeValue tv;
        util::time::getNow(tv);
        m_sendData(tv, type, bytestream, len);
    }
    void DeviceToken::sendData(util::time::TimeValue const &timestamp,
                               MessageType *type, const char *bytestream,
                               size_t len) {
        m_sendData(timestamp, type, bytestream, len);
    }

    void DeviceToken::setAsyncWaitCallback(AsyncDeviceWaitCallback const &cb) {
        AsyncDeviceToken *dev = this->asAsync();
        if (!dev) {
            throw std::logic_error(
                "Called setAsyncWaitCallback on a non-async device token!");
        }
        dev->setWaitCallback(cb);
    }

    void
    DeviceToken::setSyncUpdateCallback(SyncDeviceUpdateCallback const &cb) {
        SyncDeviceToken *dev = this->asSync();
        if (!dev) {
            throw std::logic_error(
                "Called setSyncUpdateCallback on a non-sync device token!");
        }
        dev->setUpdateCallback(cb);
    }

    void DeviceToken::connectionInteract() { m_connectionInteract(); }

    void DeviceToken::stopThreads() { m_stopThreads(); }

    ConnectionPtr DeviceToken::m_getConnection() { return m_conn; }

    ConnectionDevicePtr DeviceToken::m_getConnectionDevice() { return m_dev; }

    AsyncDeviceToken *DeviceToken::asAsync() { return nullptr; }

    SyncDeviceToken *DeviceToken::asSync() { return nullptr; }

    void DeviceToken::m_stopThreads() {}

    void DeviceToken::m_sharedInit(ConnectionPtr const &conn) {
        m_conn = conn;
        m_dev = conn->registerDevice(m_name);
        m_dev->setDeviceToken(*this);
    }

} // namespace connection
} // namespace osvr
