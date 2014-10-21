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
/* none */

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

typedef char ogvrPluginReturnCode;
#define OGVR_PLUGIN_REG_SUCCESS 0
#define OGVR_PLUGIN_REG_FAILURE 1

typedef void *ogvrPluginRegContext;
typedef void *ogvrPluginHardwarePollContext;

typedef ogvrPluginReturnCode (*ogvrHardwarePollCallback)(
    ogvrPluginHardwarePollContext, void *);

OGVR_PLUGINKIT_EXPORT ogvrPluginReturnCode
    ogvrPluginRegisterHardwarePollCallback(
        ogvrPluginRegContext ctx, ogvrHardwarePollCallback pollcallback,
        void *userdata);

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
