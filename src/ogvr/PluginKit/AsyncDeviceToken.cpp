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
    : DeviceToken(name), m_requestToSend(false), m_clearToSend(false), m_run() {
}

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
    m_ctsCond.notify_all(); // get the thread unblocked from writing
}

void AsyncDeviceToken::signalAndWaitForShutdown() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken\t"
                     "In signalAndWaitForShutdown");
    signalShutdown();
    m_run.signalAndWaitForShutdown();
    m_callbackThread.join();
}
void AsyncDeviceToken::setWaitCallback(OGVR_AsyncDeviceWaitCallback cb,
                                       void *userData) {
    m_cb = CallbackWrapper<OGVR_AsyncDeviceWaitCallback>(cb, userData);
    m_callbackThread =
        boost::thread(&AsyncDeviceToken::m_waitCallbackLoop, this);
    m_run.signalAndWaitForStart();
}
void AsyncDeviceToken::m_waitCallbackLoop() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_waitCallbackLoop starting");
    if (!m_cb) {
        return;
    }
    util::LoopGuard guard(m_run);
    while (m_run.shouldContinue()) {
        (*m_cb)();
    }
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_waitCallbackLoop exiting");
}

void AsyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                  size_t len) {
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "about to lock RTS");
    unique_lock<mutex> lockRTS(m_rtsMutex);
    assert(m_requestToSend == false);
    m_requestToSend = true;
    // Must hold on to rtsMutex until we're safely in the loop, to
    // keep connectionInteract back at the beginning

    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "about to lock CTS");
    unique_lock<mutex> lockCTS(m_ctsMutex);
    while (!m_clearToSend && m_run.shouldContinue()) {
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "In wait loop");
        if (lockRTS.owns_lock()) {
            OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                             "Unlocking RTS");
            lockRTS.unlock();
        }
        m_ctsCond.wait(lockCTS);
    }

    if (!m_run.shouldContinue()) {
        /// Quitting instead of sending -
        /// told to stop, not given permission
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "quitting instead of sending");
        return;
    }
    m_getConnectionDevice()->sendData(type, bytestream, len);
    m_requestToSend = false;
    m_sendFinished = true;
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "Notify one on sendFinishedCond");
    m_sendFinishedCond.notify_one();
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                     "done - CTS should be unlocked soon");
}

void AsyncDeviceToken::m_connectionInteract() {
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                     "About to lock RTS");
    unique_lock<mutex> lockRTS(m_rtsMutex);
    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                     "Checking RTS flag");
    if (m_requestToSend) {
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "RTS true, about to lock CTS");
        unique_lock<mutex> lockCTS(m_ctsMutex);
        m_clearToSend = true;
        m_sendFinished = false;
        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "notify all on ctsCont");
        m_ctsCond.notify_all();
        while (!m_sendFinished) {
            OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                             "In loop, waiting on sendFinishedCond");
            m_sendFinishedCond.wait(lockCTS);
        }

        OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "got sendFinished, clearing CTS");
        m_clearToSend = false;
    }

    OGVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                     "Exiting - unlocking rts.");
}

} // end of namespace ogvr