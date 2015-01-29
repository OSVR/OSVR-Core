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
#include "SyncDeviceToken.h"
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {

    SyncDeviceToken::SyncDeviceToken(std::string const &name)
        : DeviceToken(name) {}

    SyncDeviceToken::~SyncDeviceToken() {}

    void
    SyncDeviceToken::setUpdateCallback(SyncDeviceUpdateCallback const &cb) {
        OSVR_DEV_VERBOSE("In SyncDeviceToken::setUpdateCallback");
        m_cb = cb;
    }

    void SyncDeviceToken::m_sendData(util::time::TimeValue const &timestamp,
                                     MessageType *type, const char *bytestream,
                                     size_t len) {
        m_getConnectionDevice()->sendData(timestamp, type, bytestream, len);
    }

    bool
    SyncDeviceToken::m_callWhenSafeToSend(std::function<void()> &callback) {
        callback();
        return true;
    }

    void SyncDeviceToken::m_connectionInteract() {
        if (m_cb) {
            m_cb();
        }
    }

    SyncDeviceToken *SyncDeviceToken::asSync() { return this; }

} // namespace connection
} // namespace osvr