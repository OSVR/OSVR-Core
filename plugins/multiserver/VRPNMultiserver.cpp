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

std::string VRPNMultiserverData::getName(std::string const &nameStem) {
    size_t num = assignNumber(nameStem);
    std::ostringstream os;
    os << nameStem << num;
    return os.str();
}

size_t VRPNMultiserverData::assignNumber(std::string const &nameStem) {
    auto it = m_nameCount.find(nameStem);
    if (it != end(m_nameCount)) {
        it->second++;
        return it->second;
    }
    m_nameCount[nameStem] = 0;
    return 0;
}