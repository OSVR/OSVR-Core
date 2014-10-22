/** @file
    @brief Header

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

/*
// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#ifndef INCLUDED_PluginRegistrationC_h_GUID_C019DFA9_5B54_4791_B0A4_040EA20501BA
#define INCLUDED_PluginRegistrationC_h_GUID_C019DFA9_5B54_4791_B0A4_040EA20501BA

/* Internal Includes */
#include <ogvr/PluginKit/Export.h>

/* Library/third-party includes */
#include <libfunctionality/PluginInterface.h>

/* Standard includes */
/* none */

#ifdef __cplusplus
#define OGVR_C_ONLY(X)
#define OGVR_CPP_ONLY(X) X
#else
#define OGVR_C_ONLY(X) X
#define OGVR_CPP_ONLY(X)
#endif

#ifdef __cplusplus
extern "C" {
#endif
/** @name Return Codes
    @{
*/
/** @brief Return type from C API OGVR functions. */
typedef char OGVRPluginReturnCode;
/** @brief The "success" value for an OGVRPluginReturnCode */
#define OGVR_PLUGIN_REG_SUCCESS LIBFUNC_RETURN_SUCCESS
/** @brief The "failure" value for an OGVRPluginReturnCode */
#define OGVR_PLUGIN_REG_FAILURE LIBFUNC_RETURN_FAILURE
/** @} */

/** @name Opaque "context" pointers
    @brief These are passed through various methods to avoid global state.
    @{
*/
/** @brief A context pointer passed in to your plugin's entry point */
typedef void *OGVRPluginRegContext;

/** @brief A context pointer passed in to your hardware poll callback, if any.
 */
typedef void *OGVRPluginHardwarePollContext;
/** @} */

/** @name Hardware Polling
    @{
*/

/** @brief Function type of a Hardware Poll callback */
typedef OGVRPluginReturnCode (*OGVRHardwarePollCallback)(
    OGVRPluginHardwarePollContext pollContext, void *userdata);

/** @brief Register a callback in your plugin to be notified when hardware
   should be polled again.

    When your callback, a function of type OGVRHardwarePollCallback, is invoked,
   it
    will receive the same userdata you provide here (if any). Your plugin should
   do
    whatever probing necessary to detect devices you can handle and instantiate
   the
    device drivers.
*/
OGVR_PLUGINKIT_EXPORT OGVRPluginReturnCode
    ogvrPluginRegisterHardwarePollCallback(
        OGVRPluginRegContext ctx, OGVRHardwarePollCallback pollcallback,
        void *userdata);
/** @} */

/** @brief This macro begins your entry point function of your plugin. */
#define OGVR_PLUGIN(PLUGIN_NAME) LIBFUNC_PLUGIN_NO_PARAM(PLUGIN_NAME)

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#undef OGVR_CPP_ONLY
#undef OGVR_C_ONLY

#endif
