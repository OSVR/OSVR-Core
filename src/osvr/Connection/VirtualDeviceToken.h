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

#ifndef INCLUDED_VirtualDeviceToken_h_GUID_19C3D2F5_865C_4DA7_B40C_DF833D94476A
#define INCLUDED_VirtualDeviceToken_h_GUID_19C3D2F5_865C_4DA7_B40C_DF833D94476A

// Internal Includes
#include <osvr/Connection/DeviceToken.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    /// @brief A device token for a device that does not have a standard update
    /// or wait callback - for instance, the system data device.
    class VirtualDeviceToken : public OSVR_DeviceTokenObject {
      public:
        VirtualDeviceToken(std::string const &name);
        virtual ~VirtualDeviceToken();

      protected:
        /// @brief Should never be called.
        virtual void
        m_setUpdateCallback(osvr::connection::DeviceUpdateCallback const &);
        void m_sendData(util::time::TimeValue const &timestamp,
                        MessageType *type, const char *bytestream, size_t len);
        virtual GuardPtr m_getSendGuard();
        virtual void m_connectionInteract();
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_VirtualDeviceToken_h_GUID_19C3D2F5_865C_4DA7_B40C_DF833D94476A
