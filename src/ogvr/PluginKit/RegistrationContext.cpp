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
#include <ogvr/PluginKit/RegistrationContext.h>
#include "PluginSpecificRegistrationContext.h"
#include "ResetPointerList.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <algorithm>

namespace ogvr {
RegistrationContext::RegistrationContext() {}

RegistrationContext::~RegistrationContext() {
    // Reset the plugins in reverse order.
    detail::resetPointerRange(m_regMap | boost::adaptors::map_values |
                              boost::adaptors::reversed);
}

void RegistrationContext::loadPlugin(std::string const &pluginName) {
    PluginRegPtr pluginReg(new PluginSpecificRegistrationContext(pluginName));
    OGVR_DEV_VERBOSE("Plugin context created, loading plugin");
    libfunc::PluginHandle plugin =
        libfunc::loadPluginByName(pluginName, pluginReg.get());
    OGVR_DEV_VERBOSE("Plugin loaded, assuming ownership of plugin handle and "
                     "storing context");
    pluginReg->takePluginHandle(std::move(plugin));
    m_regMap.insert(std::make_pair(pluginName, pluginReg));
    OGVR_DEV_VERBOSE("Completed RegistrationContext::loadPlugin");
}

} // end of namespace ogvr
