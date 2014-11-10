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

#ifndef INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1
#define INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1

// Internal Includes
#include <ogvr/Connection/MessageType.h>

// Library/third-party includes
#include <qvrpn/vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace ogvr {
class VrpnMessageType : public MessageType {
  public:
    VrpnMessageType(std::string const &name, vrpn_ConnectionPtr &vrpnConn)
        : MessageType(name) {
        m_messageID = vrpnConn->register_message_type(name.c_str());
    }
    ~VrpnMessageType() {}

    vrpn_int32 getID() const { return m_messageID; }

  private:
    vrpn_int32 m_messageID;
};
} // namespace ogvr

#endif // INCLUDED_VrpnMessageType_h_GUID_ECDE1AD9_A386_4EA3_CA02_C5D95B16B2D1
