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

#ifndef INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2
#define INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2

// Internal Includes
#include <osvr/Connection/DeviceToken.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    class SyncDeviceToken : public OSVR_DeviceTokenObject {
      public:
        SyncDeviceToken(std::string const &name);
        virtual ~SyncDeviceToken();

      protected:
        virtual void m_setUpdateCallback(DeviceUpdateCallback const &cb);
        void m_sendData(util::time::TimeValue const &timestamp,
                        MessageType *type, const char *bytestream, size_t len);
        virtual GuardPtr m_getSendGuard();
        virtual void m_connectionInteract();

      private:
        DeviceUpdateCallback m_cb;
    };
} // namespace connection
} // namespace osvr
#endif // INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2
