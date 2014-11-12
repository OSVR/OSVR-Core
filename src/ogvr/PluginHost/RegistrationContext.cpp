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

#define OGVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include <ogvr/PluginHost/RegistrationContext.h>
#include "PluginSpecificRegistrationContextImpl.h"
#include <ogvr/Util/ResetPointerList.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/algorithm/for_each.hpp>

// Standard includes
#include <algorithm>

namespace ogvr {
namespace pluginhost {
    RegistrationContext::RegistrationContext() {}

    RegistrationContext::~RegistrationContext() {
        // Reset the plugins in reverse order.
        util::resetPointerRange(m_regMap | boost::adaptors::map_values |
                                boost::adaptors::reversed);
    }

    void RegistrationContext::loadPlugin(std::string const &pluginName) {
        PluginRegPtr pluginReg(
            PluginSpecificRegistrationContext::create(pluginName));
        pluginReg->setParent(*this);
        OGVR_DEV_VERBOSE("RegistrationContext:\t"
                         "Plugin context created, loading plugin");
        libfunc::PluginHandle plugin = libfunc::loadPluginByName(
            pluginName, pluginReg->extractOpaquePointer());
        OGVR_DEV_VERBOSE(
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
        OGVR_DEV_VERBOSE("RegistrationContext:\t"
                         "Adopted registration context for "
                         << ctx->getName());
    }

    void RegistrationContext::triggerHardwareDetect() {
        boost::for_each(m_regMap | boost::adaptors::map_values,
                        [](PluginRegPtr &pluginPtr) {
            pluginPtr->triggerHardwareDetectCallbacks();
        });
    }

    util::AnyMap &RegistrationContext::data() { return m_data; }

    util::AnyMap const &RegistrationContext::data() const { return m_data; }
} // namespace pluginhost
} // namespace ogvr
