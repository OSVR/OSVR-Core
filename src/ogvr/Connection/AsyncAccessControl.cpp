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
#include "AsyncAccessControl.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>

namespace ogvr {
namespace connection {

using boost::unique_lock;
using boost::mutex;
using boost::defer_lock;
RequestToSend::RequestToSend(AsyncAccessControl &aac)
    : m_lock(aac.m_mut, defer_lock), m_lockDone(aac.m_mutDone, defer_lock),
      m_calledRequest(false), m_rts(aac.m_rts), m_done(aac.m_done),
      m_mainMessage(aac.m_mainMessage), m_condMainThread(aac.m_condMainThread),
      m_condAsyncThread(aac.m_condAsyncThread) {}

RequestToSend::~RequestToSend() {
    BOOST_ASSERT_MSG(m_lock.owns_lock() == m_lockDone.owns_lock(),
                     "We should own either both locks or neither.");
    if (m_lock.owns_lock() && m_lockDone.owns_lock()) {
        m_rts = false;
        m_done = true;
        m_lock.unlock();
        m_lockDone.unlock();
        m_condMainThread.notify_one();
    }
}
bool RequestToSend::request() {
    BOOST_ASSERT_MSG(m_calledRequest == false, "Can only try to request once "
                                               "on a single RequestToSend "
                                               "object lifetime");
    {
        m_lock.lock();
        BOOST_ASSERT_MSG(m_rts == false, "Shouldn't happen - inconsistent "
                                         "state. Can't get in a request when "
                                         "already in one.");
        m_rts = true;
        m_done = false;
        m_calledRequest = true;
        /// Take the main thread "free to go" status lock.
        {
            m_lockDone.lock();
            m_done = false;
            while (m_mainMessage == AsyncAccessControl::MTM_WAIT) {
                m_condAsyncThread.wait(m_lock); // In here we unlock the mutex
                                                // until we are notified.
            }
            // The mutex is locked again here, until the destructor.

            // Get the return value.
            return (m_mainMessage == AsyncAccessControl::MTM_CLEAR_TO_SEND);
        }
    }
}

AsyncAccessControl::AsyncAccessControl()
    : m_rts(false), m_done(false), m_mainMessage(MTM_WAIT) {}

bool AsyncAccessControl::mainThreadCTS() {
    unique_lock<mutex> lock(m_mut);
    return m_handleRTS(lock, MTM_CLEAR_TO_SEND);
}

bool AsyncAccessControl::mainThreadDeny() {
    unique_lock<mutex> lock(m_mut);
    return m_handleRTS(lock, MTM_DENY_SEND);
}

bool AsyncAccessControl::mainThreadDenyPermanently() {
    unique_lock<mutex> lock(m_mut);
    bool handled = m_handleRTS(lock, MTM_DENY_SEND, MTM_DENY_SEND);
    if (!handled) {
        BOOST_ASSERT_MSG(lock.owns_lock(), "if m_handleRTS returns false, "
                                           "we're supposed to still own the "
                                           "lock!");
        // Even if we didn't have any RTS at the time, we still want to set the
        // state to DENY
        m_mainMessage = MTM_DENY_SEND;
    }
    return handled;
}

bool AsyncAccessControl::m_handleRTS(boost::unique_lock<boost::mutex> &lock,
                                     MainThreadMessages response,
                                     MainThreadMessages postCompletionState) {
    if (!lock.owns_lock() || (lock.mutex() != &m_mut)) {
        throw std::logic_error("m_handleRTS requires its caller to pass a lock "
                               "owning the main mutex!");
    }
    if (!m_rts) {
        return false; // No RTS to handle, so didn't handle a RTS
    }

    // In here, then, there must be an RTS.
    m_mainMessage = response;
    lock.unlock(); // Unlock to let the requestor through.
    m_condAsyncThread.notify_one();
    {
        unique_lock<mutex> finishedLock(m_mutDone);
        while (!m_done) {
            // Wait for the request to complete.
            m_condMainThread.wait(finishedLock);
        }
        // Restore wait message (or deny message if denying permanently.)
        m_mainMessage = postCompletionState;
        return true; // handled an RTS, passed-in lock now unlocked.
    }
}

} // namespace connection
} // namespace ogvr