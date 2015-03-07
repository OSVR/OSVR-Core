/** @file
    @brief Header declaring device callback types

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

#ifndef INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C
#define INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
    @{
*/
/** @brief Function type of a Device Update callback */
typedef OSVR_ReturnCode (*OSVR_DeviceUpdateCallback)(void *userData);
/** @} */

OSVR_EXTERN_C_END

#endif
