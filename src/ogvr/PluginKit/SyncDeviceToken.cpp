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
#include <ogvr/PluginKit/SyncDeviceToken.h>
#include <ogvr/PluginKit/ConnectionDevice.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
SyncDeviceToken::SyncDeviceToken(std::string const &name) : DeviceToken(name) {}

SyncDeviceToken::~SyncDeviceToken() {}

SyncDeviceToken *SyncDeviceToken::asSyncDevice() { return this; }

void SyncDeviceToken::setUpdateCallback(OGVR_SyncDeviceUpdateCallback cb,
                                        void *userData) {
    m_cb = CallbackWrapper<OGVR_SyncDeviceUpdateCallback>(cb, userData);
}

void SyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                 size_t len) {
    m_getConnectionDevice()->sendData(type, bytestream, len);
}

} // end of namespace ogvr