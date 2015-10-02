/** @file
    @brief Header providing C++ interface wrappers around functionality in
    DeviceInterfaceC.h

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

#ifndef INCLUDED_DeviceInterface_h_GUID_A929799C_02F5_4C92_C503_36C7F59D6BA1
#define INCLUDED_DeviceInterface_h_GUID_A929799C_02F5_4C92_C503_36C7F59D6BA1

// Internal Includes
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <vector>
#include <string>

namespace osvr {
namespace pluginkit {

    /** @defgroup PluginKitCppDevice Device-related functionality (C++ wrappers)
    @brief How to create and report from a device in your plugin.
    @ingroup PluginKit

    @{
    */

    /// @brief Register or recall a message type by name.
    /// @param ctx The plugin registration context received by your entry point
    /// function.
    /// @param name A unique name for the message type. The library makes a copy
    /// of this string.
    ///
    /// @throws std::runtime_error if message type registration fails
    inline OSVR_MessageType
    registerMessageType(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                        OSVR_IN_STRZ const char *name) {
        OSVR_MessageType msg;
        OSVR_ReturnCode ret = osvrDeviceRegisterMessageType(ctx, name, &msg);
        if (OSVR_RETURN_SUCCESS != ret) {
            throw std::runtime_error("Could not register message type: " +
                                     std::string(name));
        }
        return msg;
    }

    /// @overload
    inline OSVR_MessageType
    registerMessageType(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                        OSVR_IN std::string const &name) {
        if (name.empty()) {
            throw std::runtime_error(
                "Cannot register a message type with an empty name!");
        }
        return registerMessageType(ctx, name.c_str());
    }

#ifndef OSVR_DOXYGEN_EXTERNAL
    namespace detail {
        template <typename DeviceObjectType> struct UpdateTrampoline {
            static OSVR_ReturnCode update(void *userData) {
                /// @todo catch exceptions here?
                return static_cast<DeviceObjectType *>(userData)->update();
            }
        };
    } // namespace detail
#endif

    /// @brief Wrapper class for OSVR_DeviceToken
    class DeviceToken {
      public:
        /// @brief Constructor wrapping an existing device token.
        DeviceToken(OSVR_DeviceToken device) : m_dev(device) {}

        /// @brief Default constructor
        DeviceToken() : m_dev(NULL) {}

        /// @brief Conversion operator to the un-wrapped device token.
        operator OSVR_DeviceToken() const {
            m_validateToken();
            return m_dev;
        }

        /// @brief Initialize this device token as synchronous, with the given
        /// name and options.
        void initSync(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                      OSVR_IN_STRZ const char *name,
                      OSVR_IN_OPT OSVR_DeviceInitOptions options = NULL) {
            OSVR_ReturnCode ret;
            if (options) {
                ret = osvrDeviceSyncInitWithOptions(ctx, name, options, &m_dev);
            } else {
                ret = osvrDeviceSyncInit(ctx, name, &m_dev);
            }
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not initialize device token: " +
                                         std::string(name));
            }
        }

        /// @overload
        void initSync(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                      OSVR_IN std::string const &name,
                      OSVR_IN_OPT OSVR_DeviceInitOptions options = NULL) {
            if (name.empty()) {
                throw std::runtime_error("Could not initialize device token "
                                         "with an empty name field!");
            }
            initSync(ctx, name.c_str(), options);
        }

        /// @brief Initialize this device token as asynchronous, with the given
        /// name and options.
        void initAsync(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                       OSVR_IN_STRZ const char *name,
                       OSVR_IN_OPT OSVR_DeviceInitOptions options = NULL) {
            OSVR_ReturnCode ret;
            if (options) {
                ret =
                    osvrDeviceAsyncInitWithOptions(ctx, name, options, &m_dev);
            } else {
                ret = osvrDeviceAsyncInit(ctx, name, &m_dev);
            }
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not initialize device token: " +
                                         std::string(name));
            }
        }

        /// @overload
        void initAsync(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                       OSVR_IN std::string const &name,
                       OSVR_IN_OPT OSVR_DeviceInitOptions options = NULL) {
            if (name.empty()) {
                throw std::runtime_error("Could not initialize device token "
                                         "with an empty name field!");
            }
            initAsync(ctx, name.c_str(), options);
        }

        /// @brief Send a message on a registered interface type, providing the
        /// timestamp yourself
        ///
        /// Templated to call implementation in the InterfaceType class.
        ///
        /// @sa osvr::clientkit::ImagingInterface,
        /// osvr::clientkit::ImagingMessage
        template <typename InterfaceType, typename MessageType>
        void send(InterfaceType &iface, MessageType const &msg,
                  OSVR_TimeValue const &timestamp) {
            iface.send(*this, msg, timestamp);
        }

        /// @overload
        /// This version takes the current time and uses it as the timestamp.
        template <typename InterfaceType, typename MessageType>
        void send(InterfaceType &iface, MessageType const &msg) {
            send(iface, msg, util::time::getNow());
        }

        /// @brief Submit a JSON self-descriptor string for the device.
        ///
        /// @param json The JSON string to transmit.
        /// @param len The length of the string.
        ///
        /// @throws std::runtime_error if error in sending.
        void sendJsonDescriptor(OSVR_IN_READS(len) const char *json,
            OSVR_IN size_t len) {
            m_validateToken();
            OSVR_ReturnCode ret =
                osvrDeviceSendJsonDescriptor(m_dev, json, len);
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not send JSON descriptor!");
            }
        }

        /// @overload
        ///
        /// For string literals: automatically deduces the length at compile
        /// time.
        template <size_t N> void sendJsonDescriptor(const char(&json)[N]) {
            sendJsonDescriptor(json, N);
        }

        /// @overload
        void sendJsonDescriptor(OSVR_IN std::string const &json) {
            if (json.empty()) {
                throw std::runtime_error(
                    "Cannot send an empty JSON descriptor!");
            }
            sendJsonDescriptor(json.c_str(), json.length());
        }

        /// @brief Given a pointer to your object that has a public
        /// `OSVR_ReturnCode update()` method, registers that instance and
        /// method as the update callback for the device.
        ///
        /// @throws std::runtime_error if update callback registration fails
        template <typename DeviceObjectType>
        void registerUpdateCallback(OSVR_IN_PTR DeviceObjectType *object) {
            if (!object) {
                throw std::logic_error(
                    "Cannot register update callback for a null object!");
            }
            m_validateToken();
            OSVR_ReturnCode ret = osvrDeviceRegisterUpdateCallback(
                m_dev, &detail::UpdateTrampoline<DeviceObjectType>::update,
                static_cast<void *>(object));
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not register update callback!");
            }
        }

        /// @name Advanced Functionality
        /// @brief Rarely needed
        /// @{
        /// @brief Send a raw bytestream from your device with a custom message type.
        ///
        /// @note The same function is used for synchronous and asynchronous
        /// devices: the device token is sufficient to determine whether locking
        /// is needed.
        ///
        /// @param msg The registered message type.
        /// @param bytestream A string of bytes to transmit.
        /// @param len The length of the string of bytes.
        ///
        /// @throws std::runtime_error if error in sending.
        void sendData(OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN_READS(len) const char *bytestream = NULL,
                      OSVR_IN size_t len = 0) {
            m_validateToken();
            OSVR_ReturnCode ret =
                osvrDeviceSendData(m_dev, msg, bytestream, len);
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not send data!");
            }
        }

        /// @overload
        ///
        /// For string literals: automatically deduces the length at compile
        /// time.
        template <size_t N>
        void sendData(OSVR_MessageType msg, const char(&bytestream)[N]) {
            sendData(msg, bytestream, N);
        }

        /// @overload
        void sendData(OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN std::string const &bytestream) {
            if (bytestream.empty()) {
                sendData(msg);
            } else {
                sendData(msg, bytestream.data(), bytestream.length());
            }
        }

        /// @overload
        void sendData(OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN std::vector<char> const &bytestream) {
            if (bytestream.empty()) {
                sendData(msg);
            } else {
                sendData(msg, bytestream.data(), bytestream.size());
            }
        }

        /// @brief Sends a raw bytestream from your device with a known
        /// timestamp.
        ///
        /// @note The same function is used for synchronous and asynchronous
        /// devices: the device token is sufficient to determine whether locking
        /// is needed.
        ///
        /// @param timestamp The timestamp you want to associate with this
        /// message.
        /// @param msg The registered message type.
        /// @param bytestream A string of bytes to transmit.
        /// @param len The length of the string of bytes.
        ///
        /// @throws std::runtime_error if error in sending.
        void sendData(OSVR_IN OSVR_TimeValue const &timestamp,
                      OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN_READS(len) const char *bytestream = NULL,
                      OSVR_IN size_t len = 0) {
            OSVR_ReturnCode ret = osvrDeviceSendTimestampedData(
                m_dev, &timestamp, msg, bytestream, len);
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not send data!");
            }
        }

        /// @overload
        ///
        /// For string literals: automatically deduces the length at compile
        /// time.
        template <size_t N>
        void sendData(OSVR_IN OSVR_TimeValue const &timestamp,
                      OSVR_MessageType msg, const char(&bytestream)[N]) {
            sendData(timestamp, msg, bytestream, N);
        }

        /// @overload
        void sendData(OSVR_IN OSVR_TimeValue const &timestamp,
                      OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN std::string const &bytestream) {
            if (bytestream.empty()) {
                sendData(timestamp, msg);
            } else {
                sendData(timestamp, msg, bytestream.data(),
                         bytestream.length());
            }
        }

        /// @overload
        void sendData(OSVR_IN OSVR_TimeValue const &timestamp,
                      OSVR_IN_PTR OSVR_MessageType msg,
                      OSVR_IN std::vector<char> const &bytestream) {
            if (bytestream.empty()) {
                sendData(timestamp, msg);
            } else {
                sendData(timestamp, msg, bytestream.data(), bytestream.size());
            }
        }
        /// @}

      private:
        /// @brief Verifies that the user calls some init member before using
        /// other features of the token.
        void m_validateToken() const {
            if (!m_dev) {
                throw std::logic_error("Attempting an operation on a device "
                                       "token that is not yet initialized!");
            }
        }
        OSVR_DeviceToken m_dev;
    };

    /** @} */
} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_DeviceInterface_h_GUID_A929799C_02F5_4C92_C503_36C7F59D6BA1
