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
#include <ogvr/PluginKit/DeviceInterfaceC.h>
#include <ogvr/PluginKit/AnalogInterfaceC.h>
#include <ogvr/Util/PluginRegContextC.h>
#include <ogvr/PluginHost/PluginSpecificRegistrationContext_fwd.h>
#include <ogvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

// Standard includes
// - none

/// @brief Structure used internally to construct the desired type of device.
struct OGVR_DeviceInitObject : boost::noncopyable {
    OGVR_DeviceInitObject(OGVR_PluginRegContext ctx);
    /// @brief Set analogs: clears the boost::optional if 0 is passed.
    void setAnalogs(OGVR_AnalogChanCount numAnalogs);

    ogvr::pluginhost::PluginSpecificRegistrationContext &context;
    boost::optional<OGVR_AnalogChanCount> analogs;
};

namespace ogvr {
namespace pluginkit {
    typedef ::OGVR_DeviceInitObject DeviceInitObject;
} // namespace pluginkit
} // namespace ogvr

#endif // INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
