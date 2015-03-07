/** @file
    @brief Header

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

#ifndef INCLUDED_ParametersC_h_GUID_9D762A31_83E5_47A2_7040_DBD786D9A79B
#define INCLUDED_ParametersC_h_GUID_9D762A31_83E5_47A2_7040_DBD786D9A79B

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stddef.h>

OSVR_EXTERN_C_BEGIN
/** @addtogroup ClientKit
@{
*/

/** @brief Get the length of a string parameter associated with the given path.
    @param ctx Client context
    @param path A resource path (null-terminated string)
    @param[out] len The length of the string value, including null terminator. 0
   if the parameter does not exist or is not a string.

*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetStringParameterLength(OSVR_ClientContext ctx, const char path[],
                                   size_t *len);

/** @brief Get a string parameter associated with the given path.
    @param ctx Client context
    @param path A resource path (null-terminated string)
    @param[in,out] buf A buffer you allocate of the appropriate size. Must be at
   least the length returned by osvrClientGetStringParameterLength. Will contain
   the null-terminated string parameter value.
    @param len The length of the buffer you're providing. If the buffer is too
   short, an error is returned and the buffer is unchanged.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetStringParameter(OSVR_ClientContext ctx, const char path[],
                             char *buf, size_t len);

/** @} */
OSVR_EXTERN_C_END

#endif
