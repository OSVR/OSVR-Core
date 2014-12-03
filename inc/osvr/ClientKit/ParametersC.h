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
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
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
    osvrClientGetStringParameterLength(OSVR_ClientContext ctx,
                                       const char path[], size_t *len);

/** @brief Get a string parameter associated with the given path.
    @param ctx Client context
    @param path A resource path (null-terminated string)
    @param[in/out] buf A buffer you allocate of the appropriate size. Must be at
   least the length returned by osvrClientGetStringParameterLength. Will contain
   the null-terminated string parameter value. If the buffer is too short, an
   error is returned.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
    osvrClientGetStringParameter(OSVR_ClientContext ctx, const char path[],
                                 char *buf, size_t len);

/** @} */
OSVR_EXTERN_C_END

#endif
