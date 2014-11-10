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
#include "PluginSpecificRegistrationContextImpl.h"
#include <ogvr/Util/Verbosity.h>
#include <ogvr/Util/ResetPointerList.h>

// Library/third-party includes
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <stdexcept>

namespace ogvr {
namespace pluginhost {
    PluginSpecificRegistrationContextImpl::
        PluginSpecificRegistrationContextImpl(std::string const &name)
        : PluginSpecificRegistrationContext(name), m_parent(NULL) {
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         << "Creating a plugin registration context for "
                         << name);
    }

    PluginSpecificRegistrationContextImpl::
        ~PluginSpecificRegistrationContextImpl() {
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         "Destroying plugin reg context for "
                         << getName());

        // Delete the data in reverse order.
        util::resetPointerRange(m_dataList | boost::adaptors::reversed);
        m_parent = NULL; // before anything else destructs, for safety?
    }

    void PluginSpecificRegistrationContextImpl::takePluginHandle(
        libfunc::PluginHandle &handle) {
        m_handle = handle;
    }

    void PluginSpecificRegistrationContextImpl::setParent(
        RegistrationContext &parent) {
        if (m_parent != NULL && m_parent != &parent) {
            throw std::logic_error(
                "Can't set the registration context parent - already set!");
        }
        m_parent = &parent;
    }

    RegistrationContext &PluginSpecificRegistrationContextImpl::getParent() {
        if (m_parent == NULL) {
            throw std::logic_error(
                "Can't access the registration context parent - it is null!");
        }
        return *m_parent;
    }

    RegistrationContext const &
    PluginSpecificRegistrationContextImpl::getParent() const {
        if (m_parent == NULL) {
            throw std::logic_error(
                "Can't access the registration context parent - it is null!");
        }
        return *m_parent;
    }

    void PluginSpecificRegistrationContextImpl::callHardwarePollCallbacks() {
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In callHardwarePollCallbacks for "
                         << getName());

        boost::for_each(m_hardwarePollCallbacks,
                        [this](HardwarePollCallback const &f) { f(this); });
    }

    void PluginSpecificRegistrationContextImpl::registerDataWithDeleteCallback(
        OGVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
        m_dataList.emplace_back(pluginData, deleteCallback);
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_dataList.size()
                         << " data delete callbacks registered for "
                         << getName());
    }

    void PluginSpecificRegistrationContextImpl::registerHardwarePollCallback(
        OGVRHardwarePollCallback pollCallback, void *userData) {
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In registerHardwarePollCallback");
        m_hardwarePollCallbacks.emplace_back(pollCallback, userData);
        OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_hardwarePollCallbacks.size()
                         << " hardware poll callbacks registered for "
                         << getName());
    }

    util::AnyMap &PluginSpecificRegistrationContextImpl::data() {
        return m_data;
    }

    util::AnyMap const &PluginSpecificRegistrationContextImpl::data() const {
        return m_data;
    }
} // namespace pluginhost
} // namespace ogvr
