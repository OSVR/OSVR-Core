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
#include <ogvr/Client/ClientInterface.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>

using ::ogvr::client::ClientInterfacePtr;
using ::ogvr::client::ClientInterface;
using ::ogvr::make_shared;

OGVR_ClientContextObject::OGVR_ClientContextObject(const char appId[])
    : m_appId(appId) {
    OGVR_DEV_VERBOSE("Client context initialized for " << m_appId);
}

OGVR_ClientContextObject::~OGVR_ClientContextObject() {}

std::string const &OGVR_ClientContextObject::getAppId() const {
    return m_appId;
}

void OGVR_ClientContextObject::update() {
    m_update();
    std::for_each(begin(m_interfaces), end(m_interfaces),
                  [](ClientInterfacePtr const &iface) { iface->update(); });
}

ClientInterfacePtr OGVR_ClientContextObject::getInterface(const char path[]) {
    ClientInterfacePtr ret;
    if (!path) {
        return ret;
    }
    std::string p(path);
    if (p.empty()) {
        return ret;
    }
    ret = make_shared<ClientInterface>(this, path,
                                       ClientInterface::PrivateConstructor());
    m_interfaces.push_back(ret);
    return ret;
}

ClientInterfacePtr
OGVR_ClientContextObject::releaseInterface(ClientInterface *iface) {
    ClientInterfacePtr ret;
    if (!iface) {
        return ret;
    }
    InterfaceList::iterator it =
        std::find_if(begin(m_interfaces), end(m_interfaces),
                     [&](ClientInterfacePtr const &ptr) {
            if (ptr.get() == iface) {
                ret = ptr;
                return true;
            }
            return false;
        });
    BOOST_ASSERT_MSG(
        (it == end(m_interfaces)) == (!ret),
        "We should have a pointer if and only if we have the iterator");
    if (ret) {
        // Erase it from our list
        m_interfaces.erase(it);
    }
    return ret;
}