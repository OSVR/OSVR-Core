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
using osvr::pluginhost::PluginSpecificRegistrationContext;
using osvr::connection::Connection;

OSVR_DeviceInitObject::OSVR_DeviceInitObject(OSVR_PluginRegContext ctx)
    : m_context(&PluginSpecificRegistrationContext::get(ctx)),
      m_conn(Connection::retrieveConnection(m_context->getParent())),
      m_analogIface(nullptr), m_buttonIface(nullptr), m_tracker(false) {}

OSVR_DeviceInitObject::OSVR_DeviceInitObject(
    osvr::connection::ConnectionPtr conn)
    : m_context(nullptr), m_conn(conn), m_tracker(false) {}

void OSVR_DeviceInitObject::setName(std::string const &n) {
    m_name = n;
    if (m_context) {
        m_qualifiedName = m_context->getName() + "/" + m_name;
    } else {
        m_qualifiedName = m_name;
    }
}

template <typename T>
inline bool setOptional(OSVR_ChannelCount input, T ptr,
                        boost::optional<OSVR_ChannelCount> &dest) {
    if (0 == input || nullptr == ptr) {
        dest.reset();
        return false;
    }
    dest = input;
    return true;
}

void OSVR_DeviceInitObject::setAnalogs(
    OSVR_ChannelCount num, osvr::connection::AnalogServerInterface **iface) {
    if (setOptional(num, iface, m_analogs)) {
        m_analogIface = iface;
    } else {
        m_analogIface = nullptr;
    }
}

void OSVR_DeviceInitObject::returnAnalogInterface(
    osvr::connection::AnalogServerInterface &iface) {
    *m_analogIface = &iface;
}

void OSVR_DeviceInitObject::setButtons(
    OSVR_ChannelCount num, osvr::connection::ButtonServerInterface **iface) {
    if (setOptional(num, iface, m_buttons)) {
        m_buttonIface = iface;
    } else {
        m_buttonIface = nullptr;
    }
}

void OSVR_DeviceInitObject::returnButtonInterface(
    osvr::connection::ButtonServerInterface &iface) {
    *m_buttonIface = &iface;
}

void OSVR_DeviceInitObject::setTracker(
    osvr::connection::TrackerServerInterface **iface) {
    if (nullptr != iface) {
        m_tracker = true;
    } else {
        m_tracker = false;
    }
    m_trackerIface = iface;
}

void OSVR_DeviceInitObject::returnTrackerInterface(
    osvr::connection::TrackerServerInterface &iface) {
    *m_trackerIface = &iface;
}

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