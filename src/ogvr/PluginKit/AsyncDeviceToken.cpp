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
#include <ogvr/PluginKit/AsyncDeviceToken.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
AsyncDeviceToken::AsyncDeviceToken(std::string const &name)
    : DeviceToken(name) {}

AsyncDeviceToken::~AsyncDeviceToken() {}

AsyncDeviceToken *AsyncDeviceToken::asAsyncDevice() { return this; }

void AsyncDeviceToken::setWaitCallback(OGVR_AsyncDeviceWaitCallback cb,
                                       void *userData) {
    m_cb = CallbackWrapper<OGVR_AsyncDeviceWaitCallback>(cb, userData);
}

void AsyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                  size_t len) {
    /// @todo implement
}

} // end of namespace ogvr