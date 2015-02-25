/** @file
    @brief Implementation

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

// Internal Includes
#include "DeviceWrapper.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    DeviceWrapper::DeviceWrapper(std::string const &name,
                                 vrpn_ConnectionPtr const &conn, bool client)
        : vrpn_BaseClass(name.c_str(), conn.get()), m_conn(conn),
          m_client(client) {
        vrpn_BaseClass::init();
        m_setConnection(conn);
    }

    DeviceWrapper::~DeviceWrapper() {}

    RawSenderType DeviceWrapper::m_getSender() {
        return RawSenderType(d_sender_id);
    }

    void DeviceWrapper::mainloop() { update(); }

    void DeviceWrapper::m_update() {
        if (m_client) {
            client_mainloop();
            m_getConnection()->mainloop();
        } else {
            server_mainloop();
        }
    }

    int DeviceWrapper::register_types() {
        return 0; // success
    }
} // namespace common
} // namespace osvr