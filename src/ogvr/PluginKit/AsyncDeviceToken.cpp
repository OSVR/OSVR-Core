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

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
AsyncDeviceToken::AsyncDeviceToken(std::string const &name)
    : DeviceToken(name), m_done(false), m_interactionEnabled(false) {}

AsyncDeviceToken::~AsyncDeviceToken() {
    m_done = true;
#ifdef TRY_WITH_THREADS
    m_callbackThread.join();
#endif
}

AsyncDeviceToken *AsyncDeviceToken::asAsyncDevice() { return this; }

void AsyncDeviceToken::setWaitCallback(OGVR_AsyncDeviceWaitCallback cb,
                                       void *userData) {
#ifdef TRY_WITH_THREADS
    m_done = true;
    m_callbackThread.join();

    m_done = false;
    m_cb = CallbackWrapper<OGVR_AsyncDeviceWaitCallback>(cb, userData);
    m_callbackThread =
        boost::thread(&AsyncDeviceToken::m_waitCallbackLoop, this);
#endif
}
void AsyncDeviceToken::m_waitCallbackLoop() {
#ifdef TRY_WITH_THREADS
    if (!m_cb) {
        return;
    }
    while (!m_done) {
        (*m_cb)();
    }
#endif
}

void AsyncDeviceToken::m_sendData(MessageType *type, const char *bytestream,
                                  size_t len) {
#ifdef TRY_WITH_THREADS
    /// @todo block until control flow enters m_connectionInteract, then block
    /// that method while sending.
    boost::unique_lock<boost::mutex> haveDataLock(m_dataWaiting);
    boost::unique_lock<boost::mutex> checkInteractionLock(m_conditionMut);
    while (!m_interactionEnabled) {
        m_cond.wait(checkInteractionLock);
    }
#endif
}

void AsyncDeviceToken::m_connectionInteract() {
#ifdef TRY_WITH_THREADS
    boost::unique_lock<boost::mutex> checkWaiting(m_dataWaiting,
                                                  boost::try_to_lock);
    if (checkWaiting.owns_lock()) {
        // OK, then the other thread isn't waiting to interact.
        return;
    }
    {
        boost::lock_guard<boost::mutex> lock(m_conditionMut);
        m_interactionEnabled = true;
    }
    m_cond.notify_one();
    {
        boost::unique_lock<boost::mutex> lock(m_conditionMut);
        while (m_interactionEnabled) {
        }
    }
/// @todo implement
#endif
}

} // end of namespace ogvr