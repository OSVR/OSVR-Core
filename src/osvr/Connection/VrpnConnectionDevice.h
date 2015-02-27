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
