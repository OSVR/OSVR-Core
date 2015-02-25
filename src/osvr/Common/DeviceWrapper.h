/** @file
    @brief Header

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

#ifndef INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6
#define INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    class DeviceWrapper : public vrpn_BaseClass, public BaseDevice {
      public:
        DeviceWrapper(std::string const &name, vrpn_ConnectionPtr const &conn,
                      bool client);
        virtual ~DeviceWrapper();

      private:
        /// @name vrpn_BaseClass methods
        /// @{
        virtual void mainloop();
        virtual int register_types();
        /// @}
        /// @name BaseDevice methods
        /// @{
        virtual RawSenderType m_getSender();
        virtual void m_update();
        /// @}
        vrpn_ConnectionPtr m_conn;
        bool m_client;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_DeviceWrapper_h_GUID_3524C649_2276_4C87_C116_663BABDC23C6
