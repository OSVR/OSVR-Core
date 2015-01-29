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
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_DeviceInitObject::OSVR_DeviceInitObject(OSVR_PluginRegContext ctx)
    : m_context(&osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)),
      m_conn(osvr::connection::Connection::retrieveConnection(
          m_context->getParent())),
      m_tracker(false) {}
OSVR_DeviceInitObject::OSVR_DeviceInitObject(
    osvr::connection::ConnectionPtr conn)
    : m_context(nullptr), m_conn(conn), m_tracker(false) {}
void OSVR_DeviceInitObject::setName(const char *n) {
    m_name = n;
    if (m_context) {
        m_qualifiedName = m_context->getName() + "/" + m_name;
    } else {
        m_qualifiedName = m_name;
    }
}

inline void setOptional(OSVR_ChannelCount input,
                        boost::optional<OSVR_ChannelCount> &dest) {
    if (0 == input) {
        dest.reset();
    } else {
        dest = input;
    }
}

void OSVR_DeviceInitObject::setAnalogs(OSVR_ChannelCount num) {
    setOptional(num, m_analogs);
}

void OSVR_DeviceInitObject::setButtons(OSVR_ChannelCount num) {
    setOptional(num, m_buttons);
}

void OSVR_DeviceInitObject::setTracker() { m_tracker = true; }

std::string OSVR_DeviceInitObject::getQualifiedName() const {
    return m_qualifiedName;
}

osvr::connection::ConnectionPtr OSVR_DeviceInitObject::getConnection() {
    return m_conn;
}

osvr::pluginhost::PluginSpecificRegistrationContext *
OSVR_DeviceInitObject::getContext() {
    return m_context;
}