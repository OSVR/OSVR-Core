/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "PluginSpecificRegistrationContext.h"
#include <ogvr/Util/Verbosity.h>
#include "ResetPointerList.h"

// Library/third-party includes
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <stdexcept>

namespace ogvr {

PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
    std::string const &name)
    : m_name(name), m_parent(NULL) {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     << "Creating a plugin registration context for "
                     << m_name);
}

PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Destroying plugin reg context for "
                     << m_name);

    // Delete the data in reverse order.
    detail::resetPointerRange(m_dataList | boost::adaptors::reversed);
    m_parent = NULL; // before anything else destructs, for safety?
}

void PluginSpecificRegistrationContext::takePluginHandle(
    libfunc::PluginHandle &handle) {
    m_handle = handle;
}

void PluginSpecificRegistrationContext::setParent(RegistrationContext &parent) {
    if (m_parent != NULL && m_parent != &parent) {
        throw std::logic_error(
            "Can't set the registration context parent - already set!");
    }
    m_parent = &parent;
}

RegistrationContext &PluginSpecificRegistrationContext::getParent() {
    if (m_parent == NULL) {
        throw std::logic_error(
            "Can't access the registration context parent - it is null!");
    }
    return *m_parent;
}

RegistrationContext const &
PluginSpecificRegistrationContext::getParent() const {
    if (m_parent == NULL) {
        throw std::logic_error(
            "Can't access the registration context parent - it is null!");
    }
    return *m_parent;
}

const std::string &PluginSpecificRegistrationContext::getName() const {
    return m_name;
}

void PluginSpecificRegistrationContext::callHardwarePollCallbacks() {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "In callHardwarePollCallbacks for "
                     << m_name);

    boost::for_each(m_hardwarePollCallbacks,
                    [this](HardwarePollCallback const &f) { f(this); });
}

void PluginSpecificRegistrationContext::registerDataWithDeleteCallback(
    OGVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
    m_dataList.emplace_back(PluginDataPtr(pluginData, deleteCallback));
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Now have "
                     << m_dataList.size()
                     << " data delete callbacks registered for " << m_name);
}

void PluginSpecificRegistrationContext::registerHardwarePollCallback(
    OGVRHardwarePollCallback pollCallback, void *userData) {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "In registerHardwarePollCallback");
    m_hardwarePollCallbacks.emplace_back(
        CallbackWrapper<OGVRHardwarePollCallback>(pollCallback, userData));
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Now have "
                     << m_hardwarePollCallbacks.size()
                     << " hardware poll callbacks registered for " << m_name);
}

AnyMap &PluginSpecificRegistrationContext::data() { return m_data; }

AnyMap const &PluginSpecificRegistrationContext::data() const { return m_data; }

} // end of namespace ogvr
