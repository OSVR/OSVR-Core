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
    class VirtualDeviceToken : public DeviceToken {
      public:
        VirtualDeviceToken(std::string const &name);
        virtual ~VirtualDeviceToken();

      protected:
        void m_sendData(util::time::TimeValue const &timestamp,
                        MessageType *type, const char *bytestream, size_t len);
        virtual GuardPtr m_getSendGuard();
        virtual void m_connectionInteract();
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_VirtualDeviceToken_h_GUID_19C3D2F5_865C_4DA7_B40C_DF833D94476A
