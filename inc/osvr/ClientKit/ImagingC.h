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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#ifndef INCLUDED_ImagingC_h_GUID_6A9315B7_3483_42BE_6FE1_C8EF4EC59E49
#define INCLUDED_ImagingC_h_GUID_6A9315B7_3483_42BE_6FE1_C8EF4EC59E49

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ImagingReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @brief Free an image buffer returned from a callback.
    @param ctx Client context.
    @param buf Image buffer.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeImage(OSVR_ClientContext ctx, OSVR_ImageBufferElement *buf);

OSVR_EXTERN_C_END

#endif
