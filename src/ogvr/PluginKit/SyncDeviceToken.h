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
#include <ogvr/PluginKit/DeviceToken.h>
#include <ogvr/PluginKit/DeviceInterfaceC.h>
#include <ogvr/Util/CallbackWrapper.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <functional>

namespace ogvr {
class SyncDeviceToken : public DeviceToken {
  public:
    typedef std::function<OGVR_PluginReturnCode()> UpdateCallback;
    SyncDeviceToken(std::string const &name);
    virtual ~SyncDeviceToken();

    virtual SyncDeviceToken *asSyncDevice();

    void setUpdateCallback(UpdateCallback const &cb);

  protected:
    void m_sendData(MessageType *type, const char *bytestream, size_t len);
    virtual void m_connectionInteract();

  private:
    UpdateCallback m_cb;
};
} // end of namespace ogvr
#endif // INCLUDED_SyncDeviceToken_h_GUID_0A738016_90A8_4E81_B5C0_247478D59FD2
