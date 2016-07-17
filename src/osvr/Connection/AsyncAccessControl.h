/** @file
    @brief Header

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

#ifndef INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456
#define INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456

// Internal Includes
// - none

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/optional/optional.hpp>
#include <boost/thread.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    /// @brief Internal class handling the synchronization of an asynchronous
    /// thread
    /// wishing to communicate that has to wait for its turn on the communcation
    /// bus.
    class AsyncAccessControl : boost::noncopyable {
      public:
        /// @brief Constructor
        AsyncAccessControl();

        /// @name
        /// @brief Check for waiting async thread, and give it permission to
        /// send if found.
        ///
        /// Blocks until the async thread completes its work.
        /// @returns true if there was a request to send.
        bool mainThreadCTS();

        /// @brief Check for waiting async thread, and deny it permission to
        /// send if
        /// found.
        ///
        /// Blocks until the async thread concludes its request.
        /// @returns true if there was a request to send.
        bool mainThreadDeny();

        /// @brief Permanently deny present and future requests.
        /// @returns true if there was a request to send.
        bool mainThreadDenyPermanently();

      private:
        /// @brief Messages/status that may be set by the main thread for read
        /// by
        /// the async thread.
        enum MainThreadMessages {
            MTM_WAIT,
            MTM_CLEAR_TO_SEND,
            MTM_DENY_SEND,
        };

        /// @brief main mutex (for m_rts and m_mainMessage)
        typedef boost::recursive_mutex MainMutexType;
        typedef boost::unique_lock<MainMutexType> MainLockType;
        MainMutexType m_mut;

        /// @brief Shared code to handle an RTS and send a message.
        ///
        /// Blocks until the message is handled (the RTS object is destroyed),
        /// then sets the message to the given post-completion state.
        ///
        /// If an RTS was handled, the lock given will be unlocked on return.
        ///
        /// @param lock A lock owning the main mutex m_mut
        ///
        /// @returns true if an RTS was handled
        /// @throws std::logic_error if lock precondition not met
        bool m_handleRTS(MainLockType &lock, MainThreadMessages response,
                         MainThreadMessages postCompletionState = MTM_WAIT);

        /// @brief mutex keeping the main thread from running away before the
        /// async is done.
        typedef boost::mutex DoneMutexType;
        typedef boost::unique_lock<DoneMutexType> DoneLockType;
        DoneMutexType m_mutDone;

        boost::optional<boost::thread::id> m_currentRequestThread;

        /// @brief For the main thread sleep/wake awaiting completion of the
        /// async thread's work.
        boost::condition_variable m_condMainThread;
        /// @brief for the async thread's sleep/wake awaiting response from the
        /// main thread.
        boost::condition_variable_any m_condAsyncThread;

        /// Written to by async thread, read by main thread
        volatile bool m_rts;
        /// Written to by async thread, read by main thread
        volatile bool m_done;
        /// Written to by main thread, read by async thread
        volatile MainThreadMessages m_mainMessage;

        friend class RequestToSend;
    };

    /// @brief RAII object for use by an async thread to manage a request to
    /// send
    ///
    /// @todo allow timeout on RTS
    /// `enum RequestResults { RR_CTS_GRANTED, RR_DENIED, RR_TIMED_OUT };`
    class RequestToSend : boost::noncopyable {
      public:
        /// @brief Creates an RAII object to manage the request to send - note
        /// that
        /// it does _not_ immediately request to send.
        RequestToSend(AsyncAccessControl &aac);

        /// @brief Destructor
        ~RequestToSend();

        /// @brief Issues a blocking request to send.
        ///
        /// Can only be called once in the lifetime of a RequestToSend object!
        ///
        /// @returns true if request granted, false if denied.
        bool request();

        /// @brief Method to find out if this is a nested RTS - primarily for
        /// testing
        ///
        /// Only valid to call following a true return from request()
        bool isNested() const { return m_nested; }

      private:
        /// @brief Lock for AsyncAccessControl::m_mut
        AsyncAccessControl::MainLockType m_lock;

        /// @brief Lock for AsyncAccessControl::m_mutDone
        AsyncAccessControl::DoneLockType m_lockDone;

        /// @brief Has the request() method of this instance been called yet?
        bool m_calledRequest;

        /// @brief Is this a nested RTS?
        bool m_nested;

        /// @name References to AsyncAccessControl members
        /// @{
        AsyncAccessControl &m_control;
        volatile bool &m_sharedRts;
        volatile bool &m_sharedDone;
        volatile AsyncAccessControl::MainThreadMessages &m_mainMessage;

        boost::condition_variable &m_condMainThread;
        boost::condition_variable_any &m_condAsyncThread;
        /// @}
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456
