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
namespace {
    class HardwarePollCaller {
      public:
        HardwarePollCaller(PluginSpecificRegistrationContext *ctx)
            : m_ctx(ctx) {}
        template <typename F> OGVR_PluginReturnCode operator()(F func) {
            return func(static_cast<void *>(m_ctx));
        }

      private:
        PluginSpecificRegistrationContext *m_ctx;
    };
} // end of anonymous namespace

PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
    std::string const &name)
    : m_name(name) {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     << "Creating a plugin registration context for "
                     << m_name);
}

PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Destroying plugin reg context for "
                     << m_name);

    // Delete the data in reverse order.
    detail::resetPointerRange(m_dataList | boost::adaptors::reversed);
}

void PluginSpecificRegistrationContext::takePluginHandle(
    libfunc::PluginHandle &handle) {
    m_handle = handle;
}

const std::string &PluginSpecificRegistrationContext::getName() const {
    return m_name;
}

void PluginSpecificRegistrationContext::callHardwarePollCallbacks() {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "In callHardwarePollCallbacks for "
                     << m_name);

    boost::for_each(m_hardwarePollCallbacks, HardwarePollCaller(this));
}

void PluginSpecificRegistrationContext::registerDataWithDeleteCallback(
    OGVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
    m_dataList.emplace_back(PluginDataPtr(pluginData, deleteCallback));
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Now have "
                     << m_dataList.size()
                     << " data delete callbacks registered for " << m_name);
}

void PluginSpecificRegistrationContext::registerHardwarePollCallback(
    OGVRHardwarePollCallback pollCallback, void *userData) {
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "In registerHardwarePollCallback");
    m_hardwarePollCallbacks.emplace_back(
        CallbackWrapper<OGVRHardwarePollCallback>(pollCallback, userData));
    OGVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                     "Now have "
                     << m_hardwarePollCallbacks.size()
                     << " hardware poll callbacks registered for " << m_name);
}

} // end of namespace ogvr
