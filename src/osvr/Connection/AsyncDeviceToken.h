/** @file
    @brief Header

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

#ifndef INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
#define INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF

// Internal Includes
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Util/CallbackWrapper.h>
#include "AsyncAccessControl.h"

// Library/third-party includes
#include <boost/thread.hpp>
#include <boost/optional.hpp>
#include <util/RunLoopManagerBoost.h>

// Standard includes
#include <string>

namespace osvr {
namespace connection {
    class AsyncDeviceToken : public OSVR_DeviceTokenObject {
      public:
        AsyncDeviceToken(std::string const &name);
        virtual ~AsyncDeviceToken();

        void signalShutdown();
        void signalAndWaitForShutdown();

      private:
        /// @brief Registers the given "wait callback" to service the device.
        /// The thread will be launched as soon as the first connection
        /// interaction occurs.
        virtual void m_setUpdateCallback(DeviceUpdateCallback const &cb);
        /// Called from the async thread - only permitted to actually
        /// send data when m_connectionInteract says so.
        virtual void m_sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len);
        virtual GuardPtr m_getSendGuard();

        /// Called from the main thread - services requests to send from
        /// the async thread.
        virtual void m_connectionInteract();

        virtual void m_stopThreads();

        void m_ensureThreadStarted();
        DeviceUpdateCallback m_cb;
        boost::optional<boost::thread> m_callbackThread;

        AsyncAccessControl m_accessControl;

        ::util::RunLoopManagerBoost m_run;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
