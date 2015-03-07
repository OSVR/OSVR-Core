/** @file
    @brief Header declaring plugin callback types

    Must be c-safe!

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_PluginCallbackTypesC_h_GUID_40B038A2_56DB_419D_BD23_DB1747FE2F10
#define INCLUDED_PluginCallbackTypesC_h_GUID_40B038A2_56DB_419D_BD23_DB1747FE2F10

/* Internal Includes */
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/PluginRegContextC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
@{
*/
/** @brief Function type of a Plugin Data Delete callback */
typedef void (*OSVR_PluginDataDeleteCallback)(void *pluginData);

/** @brief Function type of a Hardware Detect callback */
typedef OSVR_ReturnCode (*OSVR_HardwareDetectCallback)(
    OSVR_PluginRegContext ctx, void *userData);

/** @brief Function type of a driver instantiation callback */
typedef OSVR_ReturnCode (*OSVR_DriverInstantiationCallback)(
    OSVR_PluginRegContext ctx, const char *params, void *userData);

/** @} */

OSVR_EXTERN_C_END

#endif
