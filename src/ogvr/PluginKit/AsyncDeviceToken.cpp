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
#include "AsyncDeviceToken.h"
#include <ogvr/PluginKit/ConnectionDevice.h>
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
using boost::unique_lock;
using boost::mutex;

AsyncDeviceToken::AsyncDeviceToken(std::string const &name)
    : DeviceToken(name) {}

AsyncDeviceToken::~AsyncDeviceToken() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken\t"
                     "In ~AsyncDeviceToken");

    signalAndWaitForShutdown();
}

AsyncDeviceToken *AsyncDeviceToken::asAsyncDevice() { return this; }

void AsyncDeviceToken::signalShutdown() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken\t"
                     "In signalShutdown");
    m_run.signalShutdown();
    m_accessControl.mainThreadDenyPermanently();
}

void AsyncDeviceToken::signalAndWaitForShutdown() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken\t"
                     "In signalAndWaitForShutdown");
    signalShutdown();
    m_run.signalAndWaitForShutdown();
    m_callbackThread.join();
}

namespace {
    /// @brief Function object for the wait callback loop of an AsyncDeviceToken
    class WaitCallbackLoop {
      public:
        WaitCallbackLoop(util::RunLoopManagerBase &run,
                         OGVR_AsyncDeviceWaitCallback cb, void *userData)
            : m_cb(cb, userData), m_run(&run) {}
        void operator()() {
            OGVR_DEV_VERBOSE("WaitCallbackLoop starting");
            util::LoopGuard guard(*m_run);
            while (m_run->shouldContinue()) {
                m_cb();
            }
            OGVR_DEV_VERBOSE("WaitCallbackLoop exiting");
        }

      private:
        CallbackWrapper<OGVR_AsyncDeviceWaitCallback> m_cb;
        util::RunLoopManagerBase *m_run;
    };
}

void AsyncDeviceToken::setWaitCallback(OGVR_AsyncDeviceWaitCallback cb,
                                       void *userData) {

    m_callbackThread = boost::thread(WaitCallbackLoop(m_run, cb, userData));
    m_run.signalAndWaitForStart();
}

void AsyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                  size_t len) {
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "about to create RTS object");
    RequestToSend rts(m_accessControl);

    bool clear = rts.request();
    if (!clear) {
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "RTS request responded with not clear to send.");
        return;
    }

    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "Have CTS!");
    m_getConnectionDevice()->sendData(type, bytestream, len);
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "done!");
}

void AsyncDeviceToken::m_connectionInteract() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                     "Going to send a CTS if waiting");
    bool handled = m_accessControl.mainThreadCTS();
    if (handled) {
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "Handled an RTS!");
    } else {
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "No waiting RTS!");
    }
}

} // end of namespace ogvr
