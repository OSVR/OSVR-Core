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

#ifndef INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
#define INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387

// Internal Includes
#include <osvr/Connection/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Connection/ConnectionDevicePtr.h>
#include <osvr/Util/DeviceCallbackTypesC.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <functional>

namespace osvr {
namespace connection {
    class MessageType;
    class AsyncDeviceToken;
    class SyncDeviceToken;
    class DeviceToken;
    typedef unique_ptr<DeviceToken> DeviceTokenPtr;

    typedef std::function<OSVR_ReturnCode()> AsyncDeviceWaitCallback;
    typedef std::function<OSVR_ReturnCode()> SyncDeviceUpdateCallback;

    /// @brief A DeviceToken connects the generic device interaction code in
    /// PluginKit's C API with the workings of an actual ConnectionDevice.
    class DeviceToken : boost::noncopyable {
      public:
        /// @name Factory functions
        /// @{
        /// @brief Creates a device token (and underlying ConnectionDevice) that
        /// has a wait callback that can block, that is called repeatedly in a
        /// thread of its own (managed by OSVR)
        OSVR_CONNECTION_EXPORT static DeviceTokenPtr
        createAsyncDevice(std::string const &name, ConnectionPtr const &conn);
        /// @brief Creates a device token (and underlying ConnectionDevice) that
        /// has an update method that runs in the server mainloop.
        OSVR_CONNECTION_EXPORT static DeviceTokenPtr
        createSyncDevice(std::string const &name, ConnectionPtr const &conn);
        /// @}

        /// @brief Destructor
        virtual ~DeviceToken();

        /// @brief Accessor for name property
        OSVR_CONNECTION_EXPORT std::string const &getName() const;

        /// @brief Sets the wait callback if this is an async device token.
        /// @throws std::logic_error if it isn't.
        OSVR_CONNECTION_EXPORT void
        setAsyncWaitCallback(AsyncDeviceWaitCallback const &cb);

        /// @brief Sets the update callback if this is a sync device token.
        /// @throws std::logic_error if it isn't.
        OSVR_CONNECTION_EXPORT void
        setSyncUpdateCallback(SyncDeviceUpdateCallback const &cb);

        /// @brief Send data.
        ///
        /// The timestamp for the data is assumed to be at the time this call is
        /// placed.
        ///
        /// This may block until the next connectionInteract call before
        /// forwarding on to ConnectionDevice::sendData, depending on the type
        /// of device token.
        OSVR_CONNECTION_EXPORT void
        sendData(MessageType *type, const char *bytestream, size_t len);

        /// @brief Send data.
        ///
        /// This may block until the next connectionInteract call before
        /// forwarding
        /// on to ConnectionDevice::sendData,
        /// depending on the type of device token.
        OSVR_CONNECTION_EXPORT void
        sendData(util::time::TimeValue const &timestamp, MessageType *type,
                 const char *bytestream, size_t len);

        /// @brief Interact with connection. Only legal to end up in
        /// ConnectionDevice::sendData from within here somehow.
        void connectionInteract();

        /// @brief Stop any threads spawned and owned by this DeviceToken
        void stopThreads();

      protected:
        DeviceToken(std::string const &name);
        ConnectionPtr m_getConnection();
        ConnectionDevicePtr m_getConnectionDevice();
        virtual void m_sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len) = 0;
        virtual void m_connectionInteract() = 0;
        virtual void m_stopThreads();

        virtual AsyncDeviceToken *asAsync();
        virtual SyncDeviceToken *asSync();

      private:
        void m_sharedInit(ConnectionPtr const &conn);
        std::string const m_name;
        ConnectionPtr m_conn;
        ConnectionDevicePtr m_dev;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
