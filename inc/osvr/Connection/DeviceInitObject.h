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

namespace osvr {
namespace connection {
    class AnalogServerInterface;
    class ButtonServerInterface;
    class TrackerServerInterface;
} // namespace connection
} // namespace osvr
/// @brief Structure used internally to construct the desired type of device.
struct OSVR_DeviceInitObject : boost::noncopyable {
  public:
    OSVR_CONNECTION_EXPORT explicit OSVR_DeviceInitObject(
        OSVR_PluginRegContext ctx);
    OSVR_CONNECTION_EXPORT explicit OSVR_DeviceInitObject(
        osvr::connection::ConnectionPtr conn);

    /// @brief Set the (unqualified) name of the device to create.
    OSVR_CONNECTION_EXPORT void setName(std::string const &n);

    /// @brief Set analogs: clears the boost::optional if 0 is passed.
    OSVR_CONNECTION_EXPORT void
    setAnalogs(OSVR_ChannelCount num,
               osvr::connection::AnalogServerInterface **iface);

    /// @brief Returns a pointer to an analog interface through the
    /// pointer-pointer.
    void returnAnalogInterface(osvr::connection::AnalogServerInterface *iface);

    /// @brief Set buttons: clears the boost::optional if 0 is passed.
    OSVR_CONNECTION_EXPORT void
    setButtons(OSVR_ChannelCount num,
               osvr::connection::ButtonServerInterface **iface);

    /// @brief Returns a pointer to a button interface through the
    /// pointer-pointer.
    void returnButtonInterface(osvr::connection::ButtonServerInterface *iface);

    /// @brief Enables tracker interface
    OSVR_CONNECTION_EXPORT void setTracker();

    /// @brief Get device name qualified by plugin name
    std::string getQualifiedName() const;

    /// @brief Retrieve the connection pointer.
    osvr::connection::ConnectionPtr getConnection();

    /// @brief Retrieves the plugin context
    OSVR_CONNECTION_EXPORT osvr::pluginhost::PluginSpecificRegistrationContext *
    getContext();

    boost::optional<OSVR_ChannelCount> getAnalogs() const { return m_analogs; }
    boost::optional<OSVR_ChannelCount> getButtons() const { return m_buttons; }
    bool getTracker() const { return m_tracker; }

  private:
    osvr::pluginhost::PluginSpecificRegistrationContext *m_context;
    osvr::connection::ConnectionPtr m_conn;
    std::string m_name;
    std::string m_qualifiedName;
    boost::optional<OSVR_ChannelCount> m_analogs;
    osvr::connection::AnalogServerInterface **m_analogIface;
    boost::optional<OSVR_ChannelCount> m_buttons;
    osvr::connection::ButtonServerInterface **m_buttonIface;
    bool m_tracker;
};

namespace osvr {
namespace connection {
    typedef ::OSVR_DeviceInitObject DeviceInitObject;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
