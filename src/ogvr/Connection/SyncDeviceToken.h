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

#ifndef INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2
#define INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2

// Internal Includes
#include <ogvr/Connection/DeviceToken.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>

namespace ogvr {
namespace connection {
    class SyncDeviceToken : public DeviceToken {
      public:
        SyncDeviceToken(std::string const &name);
        virtual ~SyncDeviceToken();

        void setUpdateCallback(SyncDeviceUpdateCallback const &cb);

      protected:
        void m_sendData(util::time::TimeValue const &timestamp,
                        MessageType *type, const char *bytestream, size_t len);
        virtual void m_connectionInteract();

      private:
        virtual SyncDeviceToken *asSync();
        SyncDeviceUpdateCallback m_cb;
    };
} // namespace connection
} // namespace ogvr
#endif // INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2
