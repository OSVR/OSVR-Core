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
#include <ogvr/Client/ClientInterface.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OGVR_ClientInterfaceObject::OGVR_ClientInterfaceObject(
    ::ogvr::client::ClientContext *ctx, std::string const &path,
    OGVR_ClientInterfaceObject::PrivateConstructor const &)
    : m_ctx(ctx), m_path(path) {
    OGVR_DEV_VERBOSE("Interface initialized for " << m_path);
}

std::string const &OGVR_ClientInterfaceObject::getPath() const {
    return m_path;
}

::ogvr::client::ClientContext &OGVR_ClientInterfaceObject::getContext() {
    return *m_ctx;
}
void OGVR_ClientInterfaceObject::registerCallback(OGVR_PoseCallback cb,
                                                  void *userdata) {
    using namespace std::placeholders;
    m_trackerCB.push_back(std::bind(cb, userdata, _1, _2));
}

void OGVR_ClientInterfaceObject::update() {
    OGVR_DEV_VERBOSE("Update in " << m_path);
}