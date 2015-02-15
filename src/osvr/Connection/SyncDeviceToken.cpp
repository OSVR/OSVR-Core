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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include "SyncDeviceToken.h"
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/GuardInterfaceDummy.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {

    SyncDeviceToken::SyncDeviceToken(std::string const &name)
        : OSVR_DeviceTokenObject(name) {}

    SyncDeviceToken::~SyncDeviceToken() {}

    void SyncDeviceToken::m_setUpdateCallback(DeviceUpdateCallback const &cb) {
        OSVR_DEV_VERBOSE("In SyncDeviceToken::m_setUpdateCallback");
        m_cb = cb;
    }

    void SyncDeviceToken::m_sendData(util::time::TimeValue const &timestamp,
                                     MessageType *type, const char *bytestream,
                                     size_t len) {
        m_getConnectionDevice()->sendData(timestamp, type, bytestream, len);
    }

    GuardPtr SyncDeviceToken::m_getSendGuard() {
        return GuardPtr(new util::DummyGuard);
    }

    void SyncDeviceToken::m_connectionInteract() {
        if (m_cb) {
            m_cb();
        }
    }

} // namespace connection
} // namespace osvr