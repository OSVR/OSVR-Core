/** @file
    @brief Header

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

#ifndef INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
#define INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064

// Internal Includes
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/Util/PluginRegContextC.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext_fwd.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

// Standard includes
// - none

/// @brief Structure used internally to construct the desired type of device.
struct OSVR_DeviceInitObject : boost::noncopyable {
    OSVR_DeviceInitObject(OSVR_PluginRegContext ctx);
    /// @brief Set analogs: clears the boost::optional if 0 is passed.
    void setAnalogs(OSVR_AnalogChanCount numAnalogs);

    osvr::pluginhost::PluginSpecificRegistrationContext &context;
    boost::optional<OSVR_AnalogChanCount> analogs;
};

namespace osvr {
namespace pluginkit {
    typedef ::OSVR_DeviceInitObject DeviceInitObject;
} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
