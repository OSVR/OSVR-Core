/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B
#define INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/DeviceComponentPtr.h>
#include <osvr/Common/RawMessageType.h>
#include <osvr/Common/RawSenderType.h>
#include <osvr/Common/MessageRegistration.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Common/NetworkClassOfService.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <vrpn_Connection.h>

// Standard includes
// - none

namespace osvr {
namespace common {

    /// @brief Class used as an interface for underlying devices that can have
    /// device components (corresponding to interface classes)
    class BaseDevice {
      public:
        /// @brief Virtual destructor
        OSVR_COMMON_EXPORT virtual ~BaseDevice();

        /// @brief Adds a component to a base device.
        /// @throws std::logic_error if you pass a null pointer.
        template <typename T> T *addComponent(shared_ptr<T> component) {
            T *ret = component.get();
            m_addComponent(component);
            return ret;
        }

        void registerHandler(vrpn_MESSAGEHANDLER handler, void *userdata,
                             RawMessageType const &msgType);
        void unregisterHandler(vrpn_MESSAGEHANDLER handler, void *userdata,
                               RawMessageType const &msgType);

        /// @brief Call with a MessageRegistration object, and the message type
        /// will be registered and stored in the `type` field.
        template <typename T>
        void registerMessageType(MessageRegistration<T> &messageReg) {
            messageReg.setMessageType(m_registerMessageType(T::identifier()));
        }

        RawSenderType getSender();

        /// @brief Called from the outside to run the mainloop on the device and
        /// its components.
        OSVR_COMMON_EXPORT void update();

        /// @brief Called from a component to send pending messages instead of
        /// waiting for next time.
        OSVR_COMMON_EXPORT void sendPending();

        template <typename T, typename ClassOfService>
        void packMessage(
            Buffer<T> const &buf, RawMessageType const &msgType,
            util::time::TimeValue const &timestamp,
            class_of_service::ClassOfServiceBase<ClassOfService> const &);

        template <typename T>
        void packMessage(Buffer<T> const &buf, RawMessageType const &msgType,
                         util::time::TimeValue const &timestamp);

        template <typename T, typename ClassOfService>
        void packMessage(
            Buffer<T> const &buf, RawMessageType const &msgType,
            class_of_service::ClassOfServiceBase<ClassOfService> const &);

        template <typename T>
        void packMessage(Buffer<T> const &buf, RawMessageType const &msgType);

        std::string const &getDeviceName() const;

      protected:
        /// @brief Constructor
        OSVR_COMMON_EXPORT BaseDevice();
        /// @brief Should be called by derived class to set the connection, etc.
        OSVR_COMMON_EXPORT void m_setup(vrpn_ConnectionPtr conn,
                                        RawSenderType sender,
                                        std::string const &name);
        /// @brief Accessor for underlying connection
        vrpn_ConnectionPtr m_getConnection() const;
        /// @brief Implementation-specific update (call client_mainloop() or
        /// server_mainloop() in it!)
        virtual void m_update() = 0;

      private:
        /// @brief Call with a string identifying a message type, and get back
        /// an identifier.
        RawMessageType m_registerMessageType(const char *identifier);

        OSVR_COMMON_EXPORT void m_addComponent(DeviceComponentPtr component);

        void m_packMessage(size_t len, const char *buf,
                           RawMessageType const &msgType,
                           util::time::TimeValue const &timestamp,
                           uint32_t classOfService);
        DeviceComponentList m_components;
        vrpn_ConnectionPtr m_conn;
        RawSenderType m_sender;
        std::string m_name;
    };

    template <typename T, typename ClassOfService>
    inline void BaseDevice::packMessage(
        Buffer<T> const &buf, RawMessageType const &msgType,
        util::time::TimeValue const &timestamp,
        class_of_service::ClassOfServiceBase<ClassOfService> const &) {
        m_packMessage(
            buf.size(), buf.data(), msgType, timestamp,
            class_of_service::VRPNConnectionValue<ClassOfService>::value);
    }
    template <typename T>
    inline void
    BaseDevice::packMessage(Buffer<T> const &buf, RawMessageType const &msgType,
                            util::time::TimeValue const &timestamp) {
        packMessage(buf, msgType, timestamp, class_of_service::Reliable());
    }

    template <typename T, typename ClassOfService>
    inline void BaseDevice::packMessage(
        Buffer<T> const &buf, RawMessageType const &msgType,
        class_of_service::ClassOfServiceBase<ClassOfService> const &
            classOfService) {
        util::time::TimeValue t;
        util::time::getNow(t);
        packMessage(buf, msgType, t, classOfService);
    }

    template <typename T>
    inline void BaseDevice::packMessage(Buffer<T> const &buf,
                                        RawMessageType const &msgType) {
        packMessage(buf, msgType, class_of_service::Reliable());
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B
