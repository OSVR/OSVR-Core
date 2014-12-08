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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace pluginhost {
    PluginSpecificRegistrationContextImpl::
        PluginSpecificRegistrationContextImpl(std::string const &name)
        : PluginSpecificRegistrationContext(name), m_parent(NULL) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         << "Creating a plugin registration context for "
                         << name);
    }

    PluginSpecificRegistrationContextImpl::
        ~PluginSpecificRegistrationContextImpl() {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         "Destroying plugin reg context for "
                         << getName());

        // Delete the data in reverse order.
        for (auto &ptr : m_dataList | boost::adaptors::reversed) {
            ptr.reset();
        }
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

    void
    PluginSpecificRegistrationContextImpl::triggerHardwareDetectCallbacks() {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In triggerHardwareDetectCallbacks for "
                         << getName());

        for (auto const &f : m_hardwareDetectCallbacks) {
            f(this);
        }
    }

    void PluginSpecificRegistrationContextImpl::instantiateDriver(
        const std::string &driverName, const std::string &params) const {
        auto it = m_driverInstantiationCallbacks.find(driverName);
        if (it == end(m_driverInstantiationCallbacks)) {
            throw std::logic_error("No driver initialization callback was "
                                   "registered for the driver name " +
                                   driverName);
        }
        OSVR_ReturnCode ret = (it->second)(params.c_str());
        if (ret != OSVR_RETURN_SUCCESS) {
            throw std::runtime_error("Failure returned from driver "
                                     "initialization callback by name " +
                                     driverName);
        }
    }

    void PluginSpecificRegistrationContextImpl::registerDataWithDeleteCallback(
        OSVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
        m_dataList.emplace_back(pluginData, deleteCallback);
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_dataList.size()
                         << " data delete callbacks registered for "
                         << getName());
    }

    void PluginSpecificRegistrationContextImpl::registerHardwareDetectCallback(
        OSVR_HardwareDetectCallback detectCallback, void *userData) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In registerHardwareDetectCallback");
        m_hardwareDetectCallbacks.emplace_back(detectCallback, userData);
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_hardwareDetectCallbacks.size()
                         << " hardware detect callbacks registered for "
                         << getName());
    }

    void
    PluginSpecificRegistrationContextImpl::registerDriverInstantiationCallback(
        const char *name, OSVR_DriverInstantiationCallback constructor,
        void *userData) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In registerDriverInstantiationCallback");
        std::string n(name);
        if (n.empty()) {
            throw std::logic_error("Cannot register a driver instantiation "
                                   "callback with an empty name!");
        }
        using namespace std::placeholders;
        auto it = m_driverInstantiationCallbacks.find(name);
        if (it != end(m_driverInstantiationCallbacks)) {
            throw std::logic_error("A driver initialization callback by this "
                                   "name for this plugin has already been "
                                   "registered!");
        }
        m_driverInstantiationCallbacks[name] =
            std::bind(constructor, extractOpaquePointer(), _1, userData);
    }

    util::AnyMap &PluginSpecificRegistrationContextImpl::data() {
        return m_data;
    }

    util::AnyMap const &PluginSpecificRegistrationContextImpl::data() const {
        return m_data;
    }
} // namespace pluginhost
} // namespace osvr
