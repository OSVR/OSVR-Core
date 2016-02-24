/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "AsyncAccessControl.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace connection {

    using boost::defer_lock;
    RequestToSend::RequestToSend(AsyncAccessControl &aac)
        : m_lock(aac.m_mut, defer_lock), m_lockDone(aac.m_mutDone, defer_lock),
          m_calledRequest(false), m_nested(false), m_control(aac),
          m_sharedRts(aac.m_rts), m_sharedDone(aac.m_done),
          m_mainMessage(aac.m_mainMessage),
          m_condMainThread(aac.m_condMainThread),
          m_condAsyncThread(aac.m_condAsyncThread) {}

    RequestToSend::~RequestToSend() {
        BOOST_ASSERT_MSG(m_lock.owns_lock() == m_lockDone.owns_lock(),
                         "We should own either both locks or neither.");
        if (m_lock.owns_lock() && m_lockDone.owns_lock()) {
            m_sharedRts = false;
            m_sharedDone = true;
            m_control.m_currentRequestThread.reset();
            m_lock.unlock();
            m_lockDone.unlock();
            m_condMainThread.notify_one();
        }
    }
    bool RequestToSend::request() {
        BOOST_ASSERT_MSG(m_calledRequest == false,
                         "Can only try to request once "
                         "on a single RequestToSend "
                         "object lifetime");
        {
            m_lock.lock();
            if (m_sharedRts &&
                m_control.m_currentRequestThread ==
                    boost::this_thread::get_id()) {
                /// OK, so we're recursive here. Make a note and don't add
                /// another locking layer
                m_nested = true;
                m_lock.unlock();
                return true;
            }

            BOOST_ASSERT_MSG(m_sharedRts == false,
                             "Shouldn't happen - inconsistent "
                             "state. Can't get in a request when "
                             "already in one.");
            m_sharedRts = true;
            m_sharedDone = false;
            m_calledRequest = true;
            /// Take the main thread "free to go" status lock.
            {
                m_lockDone.lock();
                m_sharedDone = false;
                while (m_mainMessage == AsyncAccessControl::MTM_WAIT) {
                    m_condAsyncThread.wait(
                        m_lock); // In here we unlock the mutex
                                 // until we are notified.
                }
                // The mutex is locked again here, until the destructor.

                // Get the return value.
                auto ret =
                    (m_mainMessage == AsyncAccessControl::MTM_CLEAR_TO_SEND);
                if (ret) {
                    BOOST_ASSERT_MSG(
                        !m_control.m_currentRequestThread.is_initialized(),
                        "Shouldn't be anyone in except us!");
                    m_control.m_currentRequestThread =
                        boost::this_thread::get_id();
                }
                return ret;
            }
        }
    }

    AsyncAccessControl::AsyncAccessControl()
        : m_rts(false), m_done(false), m_mainMessage(MTM_WAIT) {}

    bool AsyncAccessControl::mainThreadCTS() {
        MainLockType lock(m_mut);
        return m_handleRTS(lock, MTM_CLEAR_TO_SEND);
    }

    bool AsyncAccessControl::mainThreadDeny() {
        MainLockType lock(m_mut);
        return m_handleRTS(lock, MTM_DENY_SEND);
    }

    bool AsyncAccessControl::mainThreadDenyPermanently() {
        MainLockType lock(m_mut);
        bool handled = m_handleRTS(lock, MTM_DENY_SEND, MTM_DENY_SEND);
        if (!handled) {
            BOOST_ASSERT_MSG(lock.owns_lock(),
                             "if m_handleRTS returns false, "
                             "we're supposed to still own the "
                             "lock!");
            // Even if we didn't have any RTS at the time, we still want to set
            // the state to DENY
            m_mainMessage = MTM_DENY_SEND;
        }
        return handled;
    }

    bool
    AsyncAccessControl::m_handleRTS(MainLockType &lock,
                                    MainThreadMessages response,
                                    MainThreadMessages postCompletionState) {
        if (!lock.owns_lock() || (lock.mutex() != &m_mut)) {
            throw std::logic_error(
                "m_handleRTS requires its caller to pass a lock "
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
            DoneLockType finishedLock(m_mutDone);
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
} // namespace osvr
