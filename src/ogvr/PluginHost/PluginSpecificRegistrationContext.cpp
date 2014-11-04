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
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "PluginSpecificRegistrationContextImpl.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {}

PluginRegPtr
PluginSpecificRegistrationContext::create(std::string const &name) {
    PluginRegPtr pluginReg =
        make_shared<PluginSpecificRegistrationContextImpl>(name);
    return pluginReg;
}

PluginSpecificRegistrationContext &
PluginSpecificRegistrationContext::get(OGVR_PluginRegContext ctx) {
    return *static_cast<PluginSpecificRegistrationContext *>(ctx);
}

OGVR_PluginRegContext
PluginSpecificRegistrationContext::extractOpaquePointer() {
    return static_cast<OGVR_PluginRegContext>(this);
}

const std::string &PluginSpecificRegistrationContext::getName() const {
    return m_name;
}
PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
    std::string const &name)
    : m_name(name) {}

} // end of namespace ogvr
