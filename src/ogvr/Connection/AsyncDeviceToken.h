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

#ifndef INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
#define INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF

// Internal Includes
#include <ogvr/Connection/DeviceToken.h>
#include <ogvr/Util/CallbackWrapper.h>
#include "AsyncAccessControl.h"

// Library/third-party includes
#include <boost/thread.hpp>
#include <util/RunLoopManagerBoost.h>

// Standard includes
#include <string>

namespace ogvr {
namespace connection {
class AsyncDeviceToken : public DeviceToken {
  public:
    AsyncDeviceToken(std::string const &name);
    virtual ~AsyncDeviceToken();

    void signalShutdown();
    void signalAndWaitForShutdown();

    /// @brief Runs the given "wait callback" to service the device.
    void setWaitCallback(AsyncDeviceWaitCallback const &cb);

  private:
    virtual AsyncDeviceToken *asAsync();
    /// Called from the async thread - only permitted to actually
    /// send data when m_connectionInteract says so.
    virtual void m_sendData(util::time::TimeValue const &timestamp,
                            MessageType *type, const char *bytestream,
                            size_t len);
    /// Called from the main thread - services requests to send from
    /// the async thread.
    virtual void m_connectionInteract();
    boost::thread m_callbackThread;

    AsyncAccessControl m_accessControl;

    ::util::RunLoopManagerBoost m_run;
};
} // namespace connection
} // namespace ogvr

#endif // INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
