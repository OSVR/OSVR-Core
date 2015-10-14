/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_AnalysisPluginKitC_h_GUID_A33681AB_B02B_4DA1_83EF_4E4EB265E387
#define INCLUDED_AnalysisPluginKitC_h_GUID_A33681AB_B02B_4DA1_83EF_4E4EB265E387

/* Internal Includes */
#include <osvr/AnalysisPluginKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @defgroup AnalysisPluginKitC Analysis plugin functionality (base C API)
    @brief How to create an "analysis filter/plugin"
    @ingroup PluginKit
@{
*/
/** @brief Initialize a synchronous analysis device token with the options
    specified, also returning the associated client context.

    @param ctx The plugin registration context received by your entry point
    function.
    @param name A unique name for the device, abiding by the rules for an
    element (directory) in a URL. The library makes a copy of this string.
    @param options The DeviceInitOptions for your device.
     @param [out] device Will contain the unique device token assigned to your
    synchronous device.
     @param [out] clientCtx Will contain the specialized client context for your
    analysis use. It is owned by the device token, and should not be updated or
    shutdown, but can otherwise be used as provided for in @ref ClientKit.

    This transfers ownership of the DeviceInitOptions, and all created objects
    associated with it, to the returned device token.
*/
OSVR_ANALYSISPLUGINKIT_EXPORT OSVR_ReturnCode osvrAnalysisSyncInit(
    OSVR_IN_PTR OSVR_PluginRegContext ctx, OSVR_IN_STRZ const char *name,
    OSVR_IN_PTR OSVR_DeviceInitOptions options,
    OSVR_OUT_PTR OSVR_DeviceToken *device,
    OSVR_OUT_PTR OSVR_ClientContext *clientCtx)
    OSVR_FUNC_NONNULL((1, 2, 3, 4, 5));
/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
