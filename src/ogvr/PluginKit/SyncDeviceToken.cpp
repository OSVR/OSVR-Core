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
#include "SyncDeviceToken.h"
#include <ogvr/PluginKit/ConnectionDevice.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
SyncDeviceToken::SyncDeviceToken(std::string const &name) : DeviceToken(name) {}

SyncDeviceToken::~SyncDeviceToken() {}

SyncDeviceToken *SyncDeviceToken::asSyncDevice() { return this; }

void SyncDeviceToken::setUpdateCallback(UpdateCallback const &cb) {
    OGVR_DEV_VERBOSE("In SyncDeviceToken::setUpdateCallback");
    m_cb = cb;
}

void SyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                 size_t len) {
    m_getConnectionDevice()->sendData(type, bytestream, len);
}

void SyncDeviceToken::m_connectionInteract() {
    if (m_cb) {
        m_cb();
    }
}

} // end of namespace ogvr