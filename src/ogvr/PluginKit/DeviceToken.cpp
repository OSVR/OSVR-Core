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
#include <ogvr/PluginKit/AsyncDeviceToken.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
DeviceTokenPtr DeviceToken::createAsyncDevice(std::string const &name,
                                              ConnectionPtr conn) {
    DeviceTokenPtr ret(new AsyncDeviceToken(name));
    ret->m_conn = conn;
    return ret;
}

DeviceToken::DeviceToken(std::string const &name) : m_name(name) {}

DeviceToken::~DeviceToken() {}

AsyncDeviceToken *DeviceToken::asAsyncDevice() { return NULL; }

SyncDeviceToken *DeviceToken::asSyncDevice() { return NULL; }

std::string const &DeviceToken::getName() const { return m_name; }

ConnectionPtr DeviceToken::m_getConnection() { return m_conn; }

} // end of namespace ogvr
