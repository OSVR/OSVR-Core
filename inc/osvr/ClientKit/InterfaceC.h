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
/* none */

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
    @param iface The interface object
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
    osvrClientFreeInterface(OSVR_ClientInterface iface);

/** @} */
OSVR_EXTERN_C_END

#endif
