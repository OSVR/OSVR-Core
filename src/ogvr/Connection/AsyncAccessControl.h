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

#ifndef INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456
#define INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456

// Internal Includes
// - none

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

// Standard includes
// - none

namespace ogvr {
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
        /// send if
        /// found.
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
        bool m_handleRTS(boost::unique_lock<boost::mutex> &lock,
                         MainThreadMessages response,
                         MainThreadMessages postCompletionState = MTM_WAIT);

        /// @brief main mutex (for m_rts and m_mainMessage)
        boost::mutex m_mut;

        /// @brief mutex keeping the main thread from running away before the
        /// async
        /// is done.
        boost::mutex m_mutDone;

        /// @brief For the main thread sleep/wake awaiting completion of the
        /// async
        /// thread's work.
        boost::condition_variable m_condMainThread;
        /// @brief for the async thread's sleep/wake awaiting response from the
        /// main
        /// thread.
        boost::condition_variable m_condAsyncThread;

        /// Written to by async thread, read by main thread
        volatile bool m_rts;
        /// Written to by async thread, read by main thrad
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

      private:
        /// @brief Lock for AsyncAccessControl::m_mut
        boost::unique_lock<boost::mutex> m_lock;

        /// @brief Lock for AsyncAccessControl::m_mutDone
        boost::unique_lock<boost::mutex> m_lockDone;

        /// @brief Has the request() method of this instance been called yet?
        bool m_calledRequest;

        /// @name References to AsyncAccessControl members
        /// @{
        volatile bool &m_rts;
        volatile bool &m_done;
        volatile AsyncAccessControl::MainThreadMessages &m_mainMessage;

        boost::condition_variable &m_condMainThread;
        boost::condition_variable &m_condAsyncThread;
        /// @}
    };
} // namespace connection
} // namespace ogvr

#endif // INCLUDED_AsyncAccessControl_h_GUID_4255BCEE_826C_4DB4_9368_9457ADBF9456
