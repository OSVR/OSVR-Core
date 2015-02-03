/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include "AsyncDeviceToken.h"
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    using boost::unique_lock;
    using boost::mutex;

    AsyncDeviceToken::AsyncDeviceToken(std::string const &name)
        : OSVR_DeviceTokenObject(name) {}

    AsyncDeviceToken::~AsyncDeviceToken() {
        OSVR_DEV_VERBOSE("AsyncDeviceToken\t"
                         "In ~AsyncDeviceToken");

        signalAndWaitForShutdown();
    }

    void AsyncDeviceToken::signalShutdown() {
        OSVR_DEV_VERBOSE("AsyncDeviceToken\t"
                         "In signalShutdown");
        m_run.signalShutdown();
        m_accessControl.mainThreadDenyPermanently();
    }

    void AsyncDeviceToken::signalAndWaitForShutdown() {
        OSVR_DEV_VERBOSE("AsyncDeviceToken\t"
                         "In signalAndWaitForShutdown");
        signalShutdown();
        if (m_callbackThread.is_initialized()) {
            m_run.signalAndWaitForShutdown();
            m_callbackThread->join();
        }
    }

    namespace {
        /// @brief Function object for the wait callback loop of an
        /// AsyncDeviceToken
        class WaitCallbackLoop {
          public:
            WaitCallbackLoop(::util::RunLoopManagerBase &run,
                             DeviceUpdateCallback const &cb)
                : m_cb(cb), m_run(&run) {}
            void operator()() {
                OSVR_DEV_VERBOSE("WaitCallbackLoop starting");
                ::util::LoopGuard guard(*m_run);
                while (m_run->shouldContinue()) {
                    m_cb();
                }
                OSVR_DEV_VERBOSE("WaitCallbackLoop exiting");
            }

          private:
            DeviceUpdateCallback m_cb;
            ::util::RunLoopManagerBase *m_run;
        };
    } // end of anonymous namespace

    void AsyncDeviceToken::m_setUpdateCallback(DeviceUpdateCallback const &cb) {
        /// @todo enforce this can't happen when running?
        m_cb = cb;
    }
    void AsyncDeviceToken::m_ensureThreadStarted() {
        if ((!m_callbackThread.is_initialized()) && m_cb) {
            m_callbackThread = boost::thread(WaitCallbackLoop(m_run, m_cb));
            m_run.signalAndWaitForStart();
        }
    }

    void AsyncDeviceToken::m_sendData(util::time::TimeValue const &timestamp,
                                      MessageType *type, const char *bytestream,
                                      size_t len) {
        OSVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "about to create RTS object");
        RequestToSend rts(m_accessControl);

        bool clear = rts.request();
        if (!clear) {
            OSVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                             "RTS request responded with not clear to send.");
            return;
        }

        OSVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "Have CTS!");
        m_getConnectionDevice()->sendData(timestamp, type, bytestream, len);
        OSVR_DEV_VERBOSE("AsyncDeviceToken::m_sendData\t"
                         "done!");
    }

    class AsyncSendGuard : public util::GuardInterface {
      public:
        AsyncSendGuard(AsyncAccessControl &control) : m_rts(control) {}
        virtual bool lock() { return m_rts.request(); }
        virtual ~AsyncSendGuard() {}

      private:
        RequestToSend m_rts;
    };

    GuardPtr AsyncDeviceToken::m_getSendGuard() {
        return GuardPtr(new AsyncSendGuard(m_accessControl));
    }

    void AsyncDeviceToken::m_connectionInteract() {
        m_ensureThreadStarted();
        OSVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                         "Going to send a CTS if waiting");
        bool handled = m_accessControl.mainThreadCTS();
        if (handled) {
            OSVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                             "Handled an RTS!");
        } else {
            OSVR_DEV_VERBOSE("AsyncDeviceToken::m_connectionInteract\t"
                             "No waiting RTS!");
        }
    }

    void AsyncDeviceToken::m_stopThreads() { signalAndWaitForShutdown(); }

} // namespace connection
} // namespace osvr
