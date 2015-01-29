/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
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
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/PluginRegContextC.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext_fwd.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Connection/Export.h>
#include <osvr/Connection/ConnectionPtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

// Standard includes
#include <string>

/// @brief Structure used internally to construct the desired type of device.
struct OSVR_DeviceInitObject : boost::noncopyable {
    OSVR_CONNECTION_EXPORT OSVR_DeviceInitObject(OSVR_PluginRegContext ctx);

    /// @brief Set the (unqualified) name of the device to create.
    OSVR_CONNECTION_EXPORT void setName(const char *n);

    /// @brief Set analogs: clears the boost::optional if 0 is passed.
    OSVR_CONNECTION_EXPORT void setAnalogs(OSVR_ChannelCount num);

    /// @brief Set buttons: clears the boost::optional if 0 is passed.
    OSVR_CONNECTION_EXPORT void setButtons(OSVR_ChannelCount num);

    /// @brief Enables tracker interface
    OSVR_CONNECTION_EXPORT void setTracker();

    /// @brief Get device name qualified by plugin name
    std::string getQualifiedName() const;

    /// @brief Retrieve the connection pointer.
    osvr::connection::ConnectionPtr getConnection();

    /// @brief Retrieves the plugin context
    OSVR_CONNECTION_EXPORT osvr::pluginhost::PluginSpecificRegistrationContext &
    getContext();

    boost::optional<OSVR_ChannelCount> analogs;
    boost::optional<OSVR_ChannelCount> buttons;
    bool tracker;

  private:
    osvr::pluginhost::PluginSpecificRegistrationContext &m_context;
    std::string m_name;
};

namespace osvr {
namespace connection {
    typedef ::OSVR_DeviceInitObject DeviceInitObject;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
