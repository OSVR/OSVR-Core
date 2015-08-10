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

#ifndef INCLUDED_InterfaceC_h_GUID_D90BBAA6_AD62_499D_C023_2F6ED8987C17
#define INCLUDED_InterfaceC_h_GUID_D90BBAA6_AD62_499D_C023_2F6ED8987C17

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdint.h>

OSVR_EXTERN_C_BEGIN
/** @addtogroup ClientKit
@{
*/

/** @brief Get the interface associated with the given path.
    @param ctx Client context
    @param path A resource path (null-terminated string)
    @param[out] iface The interface object. May be freed when no longer needed,
   otherwise it will be freed when the context is closed.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetInterface(OSVR_ClientContext ctx, const char path[],
                       OSVR_ClientInterface *iface);

/** @brief Free an interface object before context closure.

    @param ctx Client context
    @param iface The interface object

    @returns OSVR_RETURN_SUCCESS unless a null context or interface was passed
   or the given interface was not found in the context (i.e. had already been
   freed)
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeInterface(OSVR_ClientContext ctx, OSVR_ClientInterface iface);

/** @brief Get the length of a string parameter associated with the given path.
@param ctx Client context
@param path A resource path (null-terminated string)
@param[out] len The length of the string value, including null terminator. 0
if the parameter does not exist or is not a string.

*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetNameLength(OSVR_ClientContext ctx, uint32_t id, size_t *len);

/** @brief Convert the ID to string name representation

@param ctx Client context
@param id  An id that corresponds to an entry in string to ID map
@param [in, out] buf A buffer that you allocate of appropriate size.
Must be at least the length returned by osvrClientGetStringParameterLength.
Will contain the null-terminated string parameter value.
@param len The length of the buffer you're providing. If the buffer is too
short, an error is returned and the buffer is unchanged.

@returns It will copy the name of entry name that corresponds to the
provided id
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetNameFromID(OSVR_ClientContext ctx, uint32_t id,
                        char *buf, size_t len);

/** @} */
OSVR_EXTERN_C_END

#endif
