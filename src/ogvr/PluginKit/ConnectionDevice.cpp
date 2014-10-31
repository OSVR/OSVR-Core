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
#include <ogvr/PluginKit/ConnectionDevice.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
ConnectionDevice::~ConnectionDevice() {}

std::string const &ConnectionDevice::getName() const { return m_name; }
ConnectionDevice::ConnectionDevice(std::string const &name) : m_name(name) {}

void ConnectionDevice::process() { m_process(); }

void ConnectionDevice::sendData(MessageType *type, const char *bytestream,
                                size_t len) {
    m_sendData(type, bytestream, len);
}

} // end of namespace ogvr