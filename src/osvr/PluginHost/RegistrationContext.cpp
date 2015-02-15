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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include <osvr/PluginHost/RegistrationContext.h>
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <algorithm>

namespace osvr {
namespace pluginhost {
    RegistrationContext::RegistrationContext() {}

    RegistrationContext::~RegistrationContext() {
        // Reset the plugins in reverse order.
        for (auto &ptr : m_regMap | boost::adaptors::map_values |
                             boost::adaptors::reversed) {
            ptr.reset();
        }
    }

    void RegistrationContext::loadPlugin(std::string const &pluginName) {
        PluginRegPtr pluginReg(
            PluginSpecificRegistrationContext::create(pluginName));
        pluginReg->setParent(*this);
        OSVR_DEV_VERBOSE("RegistrationContext:\t"
                         "Plugin context created, loading plugin");
        libfunc::PluginHandle plugin = libfunc::loadPluginByName(
            pluginName, pluginReg->extractOpaquePointer());
        OSVR_DEV_VERBOSE(
            "RegistrationContext:\t"
            "Plugin loaded, assuming ownership of plugin handle and "
            "storing context");
        pluginReg->takePluginHandle(plugin);
        adoptPluginRegistrationContext(pluginReg);
    }

    void RegistrationContext::adoptPluginRegistrationContext(PluginRegPtr ctx) {
        /// This set parent might be a duplicate, but won't be if the plugin reg
        /// ctx
        /// is not created by loadPlugin above.
        ctx->setParent(*this);

        m_regMap.insert(std::make_pair(ctx->getName(), ctx));
        OSVR_DEV_VERBOSE("RegistrationContext:\t"
                         "Adopted registration context for "
                         << ctx->getName());
    }

    void RegistrationContext::triggerHardwareDetect() {
        for (auto &pluginPtr : m_regMap | boost::adaptors::map_values) {
            pluginPtr->triggerHardwareDetectCallbacks();
        }
    }

    void
    RegistrationContext::instantiateDriver(const std::string &pluginName,
                                           const std::string &driverName,
                                           const std::string &params) const {
        auto pluginIt = m_regMap.find(pluginName);
        if (pluginIt == end(m_regMap)) {
            throw std::runtime_error("Could not find plugin named " +
                                     pluginName);
        }
        pluginIt->second->instantiateDriver(driverName, params);
    }

    util::AnyMap &RegistrationContext::data() { return m_data; }

    util::AnyMap const &RegistrationContext::data() const { return m_data; }
} // namespace pluginhost
} // namespace osvr
