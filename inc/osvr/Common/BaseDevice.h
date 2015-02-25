/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B
#define INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/DeviceComponentPtr.h>
#include <osvr/Common/RawMessageType.h>
#include <osvr/Common/RawSenderType.h>
#include <osvr/Common/MessageRegistration.h>
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

        /// @brief Call with a string identifying a message type, and get back
        /// an identifier.
        RawMessageType registerMessageType(const char *identifier);

        /// @brief Call with a MessageRegistration object, and the message type
        /// will be registered and stored in the `type` field.
        template <typename T>
        void registerMessageType(MessageRegistration<T> &messageReg) {
            messageReg.type = registerMessageType(T::identifier())
        }

        RawSenderType getSender();

        /// @brief Called from the outside to run the mainloop on the device and
        /// its components.
        void update();

        /// @brief Called from a component to send pending messages instead of
        /// waiting for next time.
        void sendPending();

        template <typename T, typename Alloc>
        void packMessage(std::vector<T, Alloc> const &buf,
                         util::time::TimeValue const &timestamp,
                         RawMessageType const &msgType,
                         uint32_t classOfService) {
            m_packMessage(buf.size(), buf.data(), timestamp, msgType);
        }

      protected:
        /// @brief Constructor
        BaseDevice();
        /// @brief The implementation-specific part of addComponent.
        virtual void m_addComponent(DeviceComponentPtr component);
        /// @brief Should be called by derived class to set the connection
        void m_setConnection(vrpn_ConnectionPtr conn);
        /// @brief Accessor for underlying connection
        vrpn_ConnectionPtr m_getConnection() const;
        /// @brief Implementation-specific sender retrieval
        virtual RawSenderType m_getSender() = 0;
        /// @brief Implementation-specific update (call client_mainloop() or
        /// server_mainloop() in it!)
        virtual void m_update() = 0;

      private:
        void m_addComponent(DeviceComponentPtr component);

        void m_packMessage(size_t len, const char *buf,
                           util::time::TimeValue const &timestamp,
                           RawMessageType const &msgType,
                           uint32_t classOfService);
        DeviceComponentList m_components;
        vrpn_ConnectionPtr m_conn;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_BaseDevice_h_GUID_52C0B784_DB20_4FA6_0C87_47FB116DA61B
