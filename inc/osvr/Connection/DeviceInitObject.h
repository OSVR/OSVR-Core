/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
#define INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064

// Internal Includes
#include <osvr/Connection/DeviceInitObject_fwd.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/PluginRegContextC.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Connection/Export.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Connection/ServerInterfaceList.h>
#include <osvr/Common/DeviceComponentPtr.h>
#include <osvr/Connection/DeviceInterfaceBase.h>
#include <osvr/PluginHost/RegistrationContext.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

// Standard includes
#include <string>
#include <type_traits>
#include <memory>

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

    /// @brief Returns an analog interface through the pointer-pointer.
    void returnAnalogInterface(osvr::connection::AnalogServerInterface &iface);

    /// @brief Set buttons: clears the boost::optional if 0 is passed.
    OSVR_CONNECTION_EXPORT void
    setButtons(OSVR_ChannelCount num,
               osvr::connection::ButtonServerInterface **iface);

    /// @brief Returns a button interface through the pointer-pointer.
    void returnButtonInterface(osvr::connection::ButtonServerInterface &iface);

    /// @brief Enables tracker interface
    OSVR_CONNECTION_EXPORT void
    setTracker(osvr::connection::TrackerServerInterface **iface);

    /// @brief Add a server interface pointer to our list, which will get
    /// registered when the device is created.
    OSVR_CONNECTION_EXPORT void
    addServerInterface(osvr::connection::ServerInterfacePtr const &iface);

    /// @brief Add a device component to our list, which will get added to the
    /// device when created.
    OSVR_CONNECTION_EXPORT void
    addComponent(osvr::common::DeviceComponentPtr const &comp);

    /// @brief A helper method to make a "device interface object" of
    /// user-designated type and apppropriate lifetime.
    template <typename T> T *makeInterfaceObject() {
        static_assert(
            std::is_base_of<osvr::connection::DeviceInterfaceBase, T>::value,
            "Your interface object must derive from "
            "DeviceInterfaceBase to use this handy wrapper!");
        auto ifaceObj = getContext()->registerDataWithGenericDelete(new T);
        addTokenInterest(ifaceObj);
        return ifaceObj;
    }

    /// @brief Add an observer that we'll eventually inform about the device
    /// token.
    void addTokenInterest(OSVR_DeviceTokenObject **devPtr) {
        if (nullptr != devPtr) {
            m_tokenInterest.push_back(devPtr);
        }
    }
    void addTokenInterest(osvr::connection::DeviceInterfaceBase *ifaceObj) {
        if (nullptr != ifaceObj) {
            m_deviceInterfaces.push_back(ifaceObj);
        }
    }

    /// @brief Notify all those interested what the device token is.
    void notifyToken(OSVR_DeviceTokenObject *dev) {
        for (auto interest : m_tokenInterest) {
            *interest = dev;
        }
        for (auto ifaceObj : m_deviceInterfaces) {
            ifaceObj->setDeviceToken(*dev);
        }
    }

    /// @brief Returns a tracker interface through the pointer-pointer.
    void
    returnTrackerInterface(osvr::connection::TrackerServerInterface &iface);

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
    osvr::connection::ServerInterfaceList const &getServerInterfaces() const {
        return m_serverInterfaces;
    }

    osvr::common::DeviceComponentList const &getComponents() const {
        return m_components;
    }

    OSVR_CONNECTION_EXPORT osvr::pluginhost::RegistrationContext *
    getParentContext();

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
    osvr::connection::TrackerServerInterface **m_trackerIface;
    osvr::connection::ServerInterfaceList m_serverInterfaces;
    osvr::common::DeviceComponentList m_components;
    std::vector<OSVR_DeviceTokenObject **> m_tokenInterest;

    std::vector<osvr::connection::DeviceInterfaceBase *> m_deviceInterfaces;
};

#endif // INCLUDED_DeviceInitObject_h_GUID_6B7D1689_CE30_4A9F_4B59_36773D1F0064
