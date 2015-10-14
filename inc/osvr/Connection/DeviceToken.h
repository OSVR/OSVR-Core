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

#ifndef INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
#define INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387

// Internal Includes
#include <osvr/Connection/DeviceTokenPtr.h>
#include <osvr/Connection/MessageTypePtr.h>
#include <osvr/Connection/Export.h>
#include <osvr/Connection/DeviceInitObject_fwd.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Connection/ConnectionDevicePtr.h>
#include <osvr/Util/DeviceCallbackTypesC.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/GuardPtr.h>
#include <osvr/Connection/ServerInterfaceList.h>
#include <osvr/Util/KeyedOwnershipContainer.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <functional>

namespace osvr {
namespace connection {
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

    using EventFunction = std::function<void()>;

    /// @brief Destructor
    virtual ~OSVR_DeviceTokenObject();

    /// @brief Accessor for name property
    OSVR_CONNECTION_EXPORT std::string const &getName() const;

    /// @brief Sets the update/wait callback.
    OSVR_CONNECTION_EXPORT void
    setUpdateCallback(osvr::connection::DeviceUpdateCallback const &cb);

    /// @brief Sets a function to be executed at the beginning of
    /// connectionInteract()
    OSVR_CONNECTION_EXPORT void
    setPreConnectionInteract(EventFunction const &f);

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

    OSVR_CONNECTION_EXPORT osvr::util::GuardPtr getSendGuard();

    /// @brief Interact with connection. Only legal to end up in
    /// ConnectionDevice::sendData from within here somehow.
    void connectionInteract();

    /// @brief Stop any threads spawned and owned by this DeviceToken
    void stopThreads();

    /// @brief Send a new or updated device descriptor for this device.
    OSVR_CONNECTION_EXPORT void
    setDeviceDescriptor(std::string const &jsonString);

    /// @brief Pass (smart-pointer) ownership of some object to the client
    /// context.
    template <typename T> void *acquireObject(T obj) {
        return m_ownedObjects.acquire(obj);
    }

    /// @brief Frees some object whose lifetime is controlled by the client
    /// context.
    ///
    /// @returns true if the object was found and released.
    OSVR_CONNECTION_EXPORT bool releaseObject(void *obj);

  protected:
    OSVR_DeviceTokenObject(std::string const &name);
    osvr::connection::ConnectionPtr m_getConnection();
    osvr::connection::ConnectionDevicePtr m_getConnectionDevice();
    virtual void
    m_setUpdateCallback(osvr::connection::DeviceUpdateCallback const &cb) = 0;
    virtual void m_sendData(osvr::util::time::TimeValue const &timestamp,
                            osvr::connection::MessageType *type,
                            const char *bytestream, size_t len) = 0;
    virtual osvr::util::GuardPtr m_getSendGuard() = 0;
    virtual void m_connectionInteract() = 0;
    virtual void m_stopThreads();

  private:
    void m_sharedInit(osvr::connection::DeviceInitObject &init);
    std::string const m_name;
    osvr::connection::ConnectionPtr m_conn;
    osvr::connection::ConnectionDevicePtr m_dev;
    osvr::connection::ServerInterfaceList m_serverInterfaces;
    EventFunction m_preConnectionInteract;
    osvr::util::MultipleKeyedOwnershipContainer m_ownedObjects;
};

#endif // INCLUDED_DeviceToken_h_GUID_428B015C_19A2_46B0_CFE6_CC100763D387
