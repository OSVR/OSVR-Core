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
// - none
namespace ogvr {
PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
    std::string const &name)
    : m_name(name) {
    OGVR_DEV_VERBOSE("Creating a plugin registration context for " << m_name);
}

PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {
    OGVR_DEV_VERBOSE("Destroying plugin reg context: Here's where we'd call "
                     "deleter callbacks for "
                     << m_name);

    // Delete the data in reverse order.
    detail::resetPointerRange(m_dataList | boost::adaptors::reversed);
}

void PluginSpecificRegistrationContext::takePluginHandle(
    libfunc::PluginHandle &handle) {
    m_handle = handle;
}

void PluginSpecificRegistrationContext::registerDataWithDeleteCallback(
    OGVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
    m_dataList.emplace_back(PluginDataPtr(pluginData, deleteCallback));
}

} // end of namespace ogvr
