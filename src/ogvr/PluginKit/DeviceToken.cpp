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
#include <ogvr/PluginKit/DeviceToken.h>
#include "AsyncDeviceToken.h"
#include "SyncDeviceToken.h"
#include <ogvr/PluginKit/Connection.h>
#include <ogvr/PluginKit/ConnectionDevice.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
DeviceTokenPtr DeviceToken::createAsyncDevice(std::string const &name,
                                              ConnectionPtr const &conn) {
    DeviceTokenPtr ret(new AsyncDeviceToken(name));
    ret->m_sharedInit(conn);
    return ret;
}

DeviceTokenPtr DeviceToken::createSyncDevice(std::string const &name,
                                             ConnectionPtr const &conn) {
    DeviceTokenPtr ret(new SyncDeviceToken(name));
    ret->m_sharedInit(conn);
    return ret;
}

DeviceToken::DeviceToken(std::string const &name) : m_name(name) {}

DeviceToken::~DeviceToken() {}

AsyncDeviceToken *DeviceToken::asAsyncDevice() { return NULL; }

SyncDeviceToken *DeviceToken::asSyncDevice() { return NULL; }

std::string const &DeviceToken::getName() const { return m_name; }

void DeviceToken::sendData(MessageType *type, const char *bytestream,
                           size_t len) {
    m_sendData(type, bytestream, len);
}

void DeviceToken::connectionInteract() { m_connectionInteract(); }

ConnectionPtr DeviceToken::m_getConnection() { return m_conn; }

ConnectionDevicePtr DeviceToken::m_getConnectionDevice() { return m_dev; }

void DeviceToken::m_sharedInit(ConnectionPtr const &conn) {
    m_conn = conn;
    m_dev = conn->registerDevice(m_name);
    m_dev->setDeviceToken(*this);
}

} // end of namespace ogvr
