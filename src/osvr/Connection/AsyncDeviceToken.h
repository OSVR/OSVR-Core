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

#ifndef INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
#define INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF

// Internal Includes
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Util/CallbackWrapper.h>
#include "AsyncAccessControl.h"

// Library/third-party includes
#include <boost/thread.hpp>
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
        void m_setUpdateCallback(DeviceUpdateCallback const &cb) override;
        /// Called from the async thread - only permitted to actually
        /// send data when m_connectionInteract says so.
        void m_sendData(util::time::TimeValue const &timestamp,
                                MessageType *type, const char *bytestream,
                                size_t len) override;
        GuardPtr m_getSendGuard() override;

        /// Called from the main thread - services requests to send from
        /// the async thread.
        void m_connectionInteract() override;

        void m_stopThreads() override;

        void m_ensureThreadStarted();
        DeviceUpdateCallback m_cb;
        unique_ptr<boost::thread> m_callbackThread;

        AsyncAccessControl m_accessControl;

        ::util::RunLoopManagerBoost m_run;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_AsyncDeviceToken_h_GUID_654218B0_3900_4B89_E86F_D314EB6C0ABF
