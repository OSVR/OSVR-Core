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
#include "ConnectionDevice.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace ogvr {
ConnectionDevice::~ConnectionDevice() {}

std::string const &ConnectionDevice::getName() const { return m_name; }
ConnectionDevice::ConnectionDevice(std::string const &name)
    : m_name(name), m_token(NULL) {}

void ConnectionDevice::process() { m_process(); }

void ConnectionDevice::sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len) {
    BOOST_ASSERT(type);
    m_sendData(timestamp, type, bytestream, len);
}
void ConnectionDevice::setDeviceToken(DeviceToken &token) {
    BOOST_ASSERT(m_token == NULL);
    m_token = &token;
}

DeviceToken &ConnectionDevice::m_getDeviceToken() {
    BOOST_ASSERT(m_token);
    return *m_token;
}

} // end of namespace ogvr