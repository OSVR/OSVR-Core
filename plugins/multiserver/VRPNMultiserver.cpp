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
#include "VRPNMultiserver.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <sstream>

DeviceFullName VRPNMultiserverData::getName(
    osvr::pluginhost::PluginSpecificRegistrationContext &pluginCtx,
    std::string const &nameStem) {
    std::string name = pluginCtx.getName() + "/" + assignName(nameStem);

    std::cout << "Creating " << name << std::endl;
    return DeviceFullName(name);
}

std::string VRPNMultiserverData::assignName(std::string const &nameStem) {
    size_t num = assignNumber(nameStem);
    std::ostringstream os;
    os << nameStem << num;
    return os.str();
}

size_t VRPNMultiserverData::assignNumber(std::string const &nameStem) {
    NameCountMap::iterator it = m_nameCount.find(nameStem);
    if (it != m_nameCount.end()) {
        it->second++;
        return it->second;
    }
    m_nameCount[nameStem] = 0;
    return 0;
}