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
#include <ogvr/Client/ClientContext.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ClientContextObject::OGVR_ClientContextObject(const char appId[])
    : m_appId(appId) {
    OGVR_DEV_VERBOSE("Client context initialized for " << m_appId);
}

std::string const &OGVR_ClientContextObject::getAppId() const {
    return m_appId;
}