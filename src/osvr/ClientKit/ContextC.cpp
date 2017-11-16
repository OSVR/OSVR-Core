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
#include <osvr/Client/CreateContext.h>
#include <osvr/ClientKit/ContextC.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/Tracing.h>
#include <osvr/Util/GetEnvironmentVariable.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/LogNames.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

static const char HOST_ENV_VAR[] = "OSVR_HOST";

static inline osvr::util::log::LoggerPtr make_clientkit_logger() {
    namespace log = osvr::util::log;
    return log::make_logger(log::OSVR_CLIENTKIT_LOG_NAME);
}

OSVR_ClientContext osvrClientInit(const char applicationIdentifier[],
                                  uint32_t /*flags*/) {
    auto host = osvr::util::getEnvironmentVariable(HOST_ENV_VAR);
    if (host.is_initialized()) {

        make_clientkit_logger()->notice() << "App " << applicationIdentifier
                                          << ": Connecting to non-default host "
                                          << *host;
        return ::osvr::client::createContext(applicationIdentifier,
                                             host->c_str());
    }
    make_clientkit_logger()->debug("Connecting to default (local) host");
    return ::osvr::client::createContext(applicationIdentifier);
}

OSVR_ReturnCode osvrClientCheckStatus(OSVR_ClientContext ctx) {
    if (!ctx) {
        make_clientkit_logger()->error(
            "Can't check status of a null Client Context!");
        return OSVR_RETURN_FAILURE;
    }
    return ctx->getStatus() ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}

OSVR_ClientContext osvrClientInitHost(const char applicationIdentifier[],
                                      const char host[],
                                      uint32_t /*flags*/) {

    OSVR_DEV_VERBOSE("Connecting to non-default host " << host);
    return ::osvr::client::createContext(applicationIdentifier, host);
}

OSVR_ReturnCode osvrClientUpdate(OSVR_ClientContext ctx) {
    osvr::common::tracing::ClientUpdate region;
    ctx->update();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientShutdown(OSVR_ClientContext ctx) {
    if (nullptr == ctx) {
        make_clientkit_logger()->error("Can't delete a null Client Context!");
        return OSVR_RETURN_FAILURE;
    }
    osvr::common::deleteContext(ctx);
    return OSVR_RETURN_SUCCESS;
}

void osvrClientLog(OSVR_ClientContext ctx, OSVR_LogLevel severity,
                   const char *message) {
    const auto s = static_cast<osvr::util::log::LogLevel>(severity);
    if (!ctx) {
        make_clientkit_logger()->log(s)
            << "Message from app (no client context): " << message;
        return;
    }

    ctx->log(s, message);
}
