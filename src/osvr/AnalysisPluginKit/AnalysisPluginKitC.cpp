/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Client/CreateContext.h>
#include <osvr/Util/MacroToolsC.h>
#include <osvr/Util/Verbosity.h>
#include "../Connection/VrpnConnectionKind.h" /// @todo Internal header!
#include "../Connection/VrpnConnectionKind.h" /// @todo Internal header!

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

/// @todo move this into a shared header between here and ClientKit/DisplayC.cpp
#define OSVR_VALIDATE_OUTPUT_PTR(X, DESC)                                      \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == X) {                                                        \
        OSVR_DEV_VERBOSE("Passed a null pointer for output parameter " #X      \
                         ", " DESC "!");                                       \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

class vrpn_Connection;

namespace {
	inline vrpn_Connection * extractVrpnConnection(osvr::connection::Connection & conn) {
		vrpn_Connection *ret = nullptr;
		if (std::string(conn.getConnectionKindID()) ==
			osvr::connection::getVRPNConnectionKindID()) {
			ret = static_cast<vrpn_Connection *>(conn.getUnderlyingObject());
		}
		return ret;
	}
} // namespace

OSVR_ReturnCode
osvrAnalysisSyncInit(OSVR_IN_PTR OSVR_PluginRegContext ctx,
                     OSVR_IN_STRZ const char *name,
                     OSVR_IN_PTR OSVR_DeviceInitOptions options,
                     OSVR_OUT_PTR OSVR_DeviceToken *device,
                     OSVR_OUT_PTR OSVR_ClientContext *clientCtx) {
    if (!ctx) {
        OSVR_DEV_VERBOSE("osvrAnalysisSyncInit: can't use a null plugin "
                         "registration context.");
        return OSVR_RETURN_FAILURE;
    }
    if (!name || !(name[0])) {
        OSVR_DEV_VERBOSE(
            "osvrAnalysisSyncInit: can't use a null or empty device name.");
        return OSVR_RETURN_FAILURE;
    }
    OSVR_VALIDATE_OUTPUT_PTR(device, "device token");
    OSVR_VALIDATE_OUTPUT_PTR(clientCtx, "client context");

    auto initialResult =
        osvrDeviceSyncInitWithOptions(ctx, name, options, device);
    if (initialResult == OSVR_RETURN_FAILURE) {
        OSVR_DEV_VERBOSE(
            "osvrAnalysisSyncInit: couldn't create initial device token.");
        return OSVR_RETURN_FAILURE;
    }

    /// Dig the VRPN connection out of the server.
    auto osvrConn = osvr::connection::Connection::retrieveConnection(
        osvr::pluginhost::PluginSpecificRegistrationContext::get(ctx)
            .getParent());
    auto vrpnConn = extractVrpnConnection(*osvrConn);

    /// Create a client context here

    /// @todo Use an interface factory that handles relative paths.
    auto clientCtxSmart = osvr::common::wrapSharedContext(
        osvr::client::createAnalysisClientContext(
            "org.osvr.analysisplugin" /**< @todo */, "localhost" /**< @todo */,
            vrpn_ConnectionPtr(vrpnConn)));
    auto & dev = **device;
    /// pass ownership
    dev.acquireObject(clientCtxSmart);

    /// register client context update
	dev.setPreConnectionInteract([=] { clientCtxSmart->update(); });

    /// finally return the client context too.
    *clientCtx = clientCtxSmart.get();
    return OSVR_RETURN_SUCCESS;
}
