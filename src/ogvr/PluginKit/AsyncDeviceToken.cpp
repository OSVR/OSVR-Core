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
#include <ogvr/PluginKit/AsyncDeviceToken.h>
#include <ogvr/PluginKit/ConnectionDevice.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
using boost::unique_lock;
using boost::mutex;

AsyncDeviceToken::AsyncDeviceToken(std::string const &name)
    : DeviceToken(name), m_requestToSend(false), m_clearToSend(false),
      m_done(false) {}

AsyncDeviceToken::~AsyncDeviceToken() {
    m_done = true;
    m_callbackThread.join();
}

AsyncDeviceToken *AsyncDeviceToken::asAsyncDevice() { return this; }

void AsyncDeviceToken::setWaitCallback(OGVR_AsyncDeviceWaitCallback cb,
                                       void *userData) {
    m_done = true;
    m_callbackThread.join();

    m_done = false;
    m_cb = CallbackWrapper<OGVR_AsyncDeviceWaitCallback>(cb, userData);
    m_callbackThread =
        boost::thread(&AsyncDeviceToken::m_waitCallbackLoop, this);
}
void AsyncDeviceToken::m_waitCallbackLoop() {
    if (!m_cb) {
        return;
    }
    while (!m_done) {
        (*m_cb)();
    }
}

void AsyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                  size_t len) {
    unique_lock<mutex> lockRTS(m_rtsMutex);
    assert(m_requestToSend == false);
    m_requestToSend = true;
    // Must hold on to rtsMutex until we're safely in the loop, to
    // keep connectionInteract back at the beginning
    unique_lock<mutex> lockCTS(m_ctsMutex);
    while (!m_clearToSend) {
        if (lockRTS.owns_lock()) {
            lockRTS.unlock();
        }
        m_ctsCond.wait(lockCTS);
    }
    m_getConnectionDevice()->sendData(type, bytestream, len);
    m_requestToSend = false;
    m_sendFinished = true;
    m_sendFinishedCond.notify_one();
}

void AsyncDeviceToken::m_connectionInteract() {
    unique_lock<mutex> lockRTS(m_rtsMutex);
    if (m_requestToSend) {
        unique_lock<mutex> lockCTS(m_ctsMutex);
        m_clearToSend = true;
        m_sendFinished = false;
        m_ctsCond.notify_all();
        while (!m_sendFinished) {
            m_sendFinishedCond.wait(lockCTS);
        }
        m_clearToSend = false;
    }
}

} // end of namespace ogvr