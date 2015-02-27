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

#ifndef INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
#define INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387

// Internal Includes
#include <osvr/Connection/DeviceTokenPtr.h>
#include <osvr/Connection/MessageTypePtr.h>
#include <osvr/Connection/Export.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Connection/ConnectionDevicePtr.h>
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Util/DeviceCallbackTypesC.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/GuardInterface.h>
#include <osvr/Connection/ServerInterfaceList.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <functional>

namespace osvr {
namespace connection {
    typedef unique_ptr<util::GuardInterface> GuardPtr;
    typedef std::function<OSVR_ReturnCode()> DeviceUpdateCallback;
} // namespace connection
} // namespace osvr

/// @brief A DeviceToken connects the generic device interaction code in
/// PluginKit's C API with the workings of an actual ConnectionDevice.
struct OSVR_DeviceTokenObject : boost::noncopyable {
  public:
    /// @name Factory functions
    /// @{
    /// @brief Creates a device token (and underlying ConnectionDevice) that
    /// has a wait callback that can block, that is called repeatedly in a
    /// thread of its own (managed by OSVR)
    OSVR_CONNECTION_EXPORT static osvr::connection::DeviceTokenPtr
    createAsyncDevice(osvr::connection::DeviceInitObject &init);
    /// @brief Creates a device token (and underlying ConnectionDevice) that
    /// has an update method that runs in the server mainloop.
    OSVR_CONNECTION_EXPORT static osvr::connection::DeviceTokenPtr
    createSyncDevice(osvr::connection::DeviceInitObject &init);
    /// @brief Creates a device token (and underlying ConnectionDevice)
    /// without a traditional, built-in update method - typically for
    /// server-internal usage.
    OSVR_CONNECTION_EXPORT static osvr::connection::DeviceTokenPtr
    createVirtualDevice(std::string const &name,
                        osvr::connection::ConnectionPtr const &conn);
    /// @}

    /// @brief Destructor
    virtual ~OSVR_DeviceTokenObject();

    /// @brief Accessor for name property
    OSVR_CONNECTION_EXPORT std::string const &getName() const;

    /// @brief Sets the update/wait callback.
    OSVR_CONNECTION_EXPORT void
    setUpdateCallback(osvr::connection::DeviceUpdateCallback const &cb);

    /// @brief Send data.
    ///
    /// The timestamp for the data is assumed to be at the time this call is
    /// placed.
    ///
    /// This may block until the next connectionInteract call before
    /// forwarding on to ConnectionDevice::sendData, depending on the type
    /// of device token.
    OSVR_CONNECTION_EXPORT void sendData(osvr::connection::MessageType *type,
                                         const char *bytestream, size_t len);

    /// @brief Send data.
    ///
    /// This may block until the next connectionInteract call before
    /// forwarding on to ConnectionDevice::sendData, depending on the type
    /// of device token.
    OSVR_CONNECTION_EXPORT void
    sendData(osvr::util::time::TimeValue const &timestamp,
             osvr::connection::MessageType *type, const char *bytestream,
             size_t len);

    OSVR_CONNECTION_EXPORT osvr::connection::GuardPtr getSendGuard();

    /// @brief Interact with connection. Only legal to end up in
    /// ConnectionDevice::sendData from within here somehow.
    void connectionInteract();

    /// @brief Stop any threads spawned and owned by this DeviceToken
    void stopThreads();

  protected:
    OSVR_DeviceTokenObject(std::string const &name);
    osvr::connection::ConnectionPtr m_getConnection();
    osvr::connection::ConnectionDevicePtr m_getConnectionDevice();
    virtual void
    m_setUpdateCallback(osvr::connection::DeviceUpdateCallback const &cb) = 0;
    virtual void m_sendData(osvr::util::time::TimeValue const &timestamp,
                            osvr::connection::MessageType *type,
                            const char *bytestream, size_t len) = 0;
    virtual osvr::connection::GuardPtr m_getSendGuard() = 0;
    virtual void m_connectionInteract() = 0;
    virtual void m_stopThreads();

  private:
    void m_sharedInit(osvr::connection::DeviceInitObject &init);
    std::string const m_name;
    osvr::connection::ConnectionPtr m_conn;
    osvr::connection::ConnectionDevicePtr m_dev;
    osvr::connection::ServerInterfaceList m_serverInterfaces;
};

#endif // INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
