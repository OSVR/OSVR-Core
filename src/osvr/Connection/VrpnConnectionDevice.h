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

#ifndef INCLUDED_VrpnConnectionDevice_h_GUID_A5D057AB_D92A_4784_885E_3DAAAEE142A5
#define INCLUDED_VrpnConnectionDevice_h_GUID_A5D057AB_D92A_4784_885E_3DAAAEE142A5

// Internal Includes
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Util/UniquePtr.h>
#include "VrpnBaseFlexServer.h"
#include "GenerateVrpnDynamicServer.h"

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>

namespace osvr {
namespace connection {
    /// @brief ConnectionDevice implementation for a VrpnBasedConnection
    class VrpnConnectionDevice : public ConnectionDevice {
      public:
        VrpnConnectionDevice(DeviceInitObject &init,
                             vrpn_ConnectionPtr const &vrpnConn)
            : ConnectionDevice(init.getQualifiedName()) {
            DeviceConstructionData data(init, vrpnConn.get());
            m_server.reset(generateVrpnDynamicServer(data));
            m_baseobj = data.flexServer;
            for (auto const &component : init.getComponents()) {
                m_baseobj->addComponent(component);
            }
        }
        virtual ~VrpnConnectionDevice() {}
        virtual void m_process() {
            m_getDeviceToken().connectionInteract();
            m_server->mainloop();
            m_baseobj->mainloop();
        }
        virtual void m_sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len) {
            VrpnMessageType *msgtype = static_cast<VrpnMessageType *>(type);
            m_baseobj->sendData(timestamp, msgtype->getID(), bytestream, len);
        }

      private:
        vrpn_BaseFlexServer *m_baseobj;
        unique_ptr<vrpn_MainloopObject> m_server;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnConnectionDevice_h_GUID_A5D057AB_D92A_4784_885E_3DAAAEE142A5
