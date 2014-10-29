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
#include <ogvr/PluginKit/CreatePluginSpecificRegistrationContext.h>
#include "PluginSpecificRegistrationContext.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
PluginRegPtr createPluginSpecificRegistrationContext(std::string const &name) {
    PluginRegPtr pluginReg =
        make_shared<PluginSpecificRegistrationContext>(name);
    return pluginReg;
}

OGVR_PluginRegContext
extractPluginRegistrationContext(PluginRegPtr const &ctx) {
    return static_cast<OGVR_PluginRegContext>(ctx.get());
}
} // end of namespace ogvr