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
#include <osvr/ClientKit/ContextC.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Client/CreateContext.h>
#include <osvr/Client/LocateServer.h>
#include <osvr/Common/GetEnvironmentVariable.h>
#include <osvr/Common/Tracing.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/PlatformConfig.h>
#include <osvr/Util/ProcessUtils.h>

// Library/third-party includes
#if !defined(OSVR_ANDROID)
#if defined(OSVR_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif
#endif

// Standard includes
// - none

OSVR_ReturnCode osvrServerAutoStart()
{
    // @todo start the server.
#if defined(OSVR_ANDROID)
    // @todo implement auto-start for android
#else
    auto server = osvr::client::getServerBinaryDirectoryPath();
    if (server.is_initialized()) {
        OSVR_DEV_VERBOSE("Attempting to auto-start OSVR server from path " << *server);
        auto exePath = osvr::client::getServerLauncherBinaryPath();
        if (!exePath.is_initialized()) {
            OSVR_DEV_VERBOSE("No server launcher binary available.");
            return OSVR_RETURN_FAILURE;
        }

        if (osvrStartProcess(exePath->c_str(), server->c_str()) == OSVR_RETURN_FAILURE) {
            OSVR_DEV_VERBOSE("Could not auto-start server process.");
            return OSVR_RETURN_FAILURE;
        }
    } else {
        OSVR_DEV_VERBOSE("The current server location is currently unknown. Assuming server is already running.");
    }
#endif
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrServerRelease()
{
#if defined(OSVR_ANDROID)
    // @todo cleanup server thread
#else
    // no-op. Leave the server running.
#endif
    return OSVR_RETURN_SUCCESS;
}

static const char HOST_ENV_VAR[] = "OSVR_HOST";

OSVR_ClientContext osvrClientInit(const char applicationIdentifier[],
                                  uint32_t /*flags*/) {
    auto host = osvr::common::getEnvironmentVariable(HOST_ENV_VAR);
    if (host.is_initialized()) {
        OSVR_DEV_VERBOSE("Connecting to non-default host " << *host);
        return ::osvr::client::createContext(applicationIdentifier,
                                             host->c_str());
    }
    OSVR_DEV_VERBOSE("Connecting to default (local) host");
    return ::osvr::client::createContext(applicationIdentifier);
}
OSVR_ReturnCode osvrClientCheckStatus(OSVR_ClientContext ctx) {
    if (!ctx) {
        return OSVR_RETURN_FAILURE;
    }
    return ctx->getStatus() ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}
OSVR_ReturnCode osvrClientUpdate(OSVR_ClientContext ctx) {
    osvr::common::tracing::ClientUpdate region;
    ctx->update();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientShutdown(OSVR_ClientContext ctx) {
    if (nullptr == ctx) {
        OSVR_DEV_VERBOSE("Can't delete a null Client Context!");
        return OSVR_RETURN_FAILURE;
    }
    osvr::common::deleteContext(ctx);
    return OSVR_RETURN_SUCCESS;
}
