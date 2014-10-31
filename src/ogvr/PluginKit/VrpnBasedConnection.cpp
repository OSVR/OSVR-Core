/** @file
    @brief Implementation

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

// Internal Includes
#include "VrpnBasedConnection.h"
#include "VrpnMessageType.h"
#include <ogvr/PluginKit/ConnectionDevice.h>
#include <ogvr/Util/UniquePtr.h>

#include <vrpn_BaseClass.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {

namespace {
    class vrpn_BaseFlexServer : public vrpn_BaseClass {
      public:
        vrpn_BaseFlexServer(const char *name, vrpn_Connection *conn)
            : vrpn_BaseClass(name, conn) {
            vrpn_BaseClass::init();
        }
        virtual ~vrpn_BaseFlexServer() {}

        virtual void mainloop() {
            /// @todo service device here
            server_mainloop();
        }

      protected:
        virtual int register_types() { return 0; }
    };

    class VrpnConnectionDevice : public ConnectionDevice {
      public:
        VrpnConnectionDevice(std::string const &name,
                             vrpn_ConnectionPtr const &vrpnConn)
            : ConnectionDevice(name) {
            m_baseobj.reset(
                new vrpn_BaseFlexServer(name.c_str(), vrpnConn.get()));
        }
        virtual ~VrpnConnectionDevice() {}
        virtual void m_process() { m_baseobj->mainloop(); }

      private:
        unique_ptr<vrpn_BaseFlexServer> m_baseobj;
    };
}

VrpnBasedConnection::VrpnBasedConnection(ConnectionType type) {
    switch (type) {
    case VRPN_LOCAL_ONLY: {
        m_vrpnConnection =
            vrpn_ConnectionPtr::create_server_connection("127.0.0.1");
    }
    case VRPN_SHARED: {
        m_vrpnConnection = vrpn_ConnectionPtr::create_server_connection();
    }
    }
}

MessageTypePtr
VrpnBasedConnection::m_registerMessageType(std::string const &messageId) {
    MessageTypePtr ret(new VrpnMessageType(messageId, m_vrpnConnection));
    return ret;
}

ConnectionDevicePtr
VrpnBasedConnection::m_registerDevice(std::string const &deviceName) {
    ConnectionDevicePtr ret =
        make_shared<VrpnConnectionDevice>(deviceName, m_vrpnConnection);
    return ret;
}

void VrpnBasedConnection::m_process() { m_vrpnConnection->mainloop(); }

VrpnBasedConnection::~VrpnBasedConnection() {}
}