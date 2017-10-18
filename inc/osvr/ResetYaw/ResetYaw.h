/** @file
    @brief Header

    @date 2017

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2017 Sensics, Inc.
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

#ifndef INCLUDED_ResetYaw_h_GUID_75486478_DD8B_4697_1FD8_0BEEF82DBB75
#define INCLUDED_ResetYaw_h_GUID_75486478_DD8B_4697_1FD8_0BEEF82DBB75


/* Internal Includes */
#include <osvr/ResetYaw/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

OSVR_RESETYAW_EXPORT OSVR_ReturnCode osvrResetYaw(OSVR_ClientContext ctx);

OSVR_EXTERN_C_END

#endif