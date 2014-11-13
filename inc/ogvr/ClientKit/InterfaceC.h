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
#include <ogvr/ClientKit/Export.h>
#include <ogvr/Util/APIBaseC.h>
#include <ogvr/Util/ReturnCodesC.h>
#include <ogvr/Util/AnnotationMacrosC.h>
#include <ogvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN
/** @addtogroup ClientKit
@{
*/

/** @brief Get the interface associated with the given path.
    @param ctx Client context
    @param path A resource path (null-terminated string)
    @param[out] iface The interface object. May be freed when no longer needed,
   otherwise it will be freed when the context is closed.
*/
OGVR_CLIENTKIT_EXPORT OGVR_ReturnCode
    ogvrClientGetInterface(OGVR_ClientContext ctx, const char path[],
                           OGVR_ClientInterface *iface);

/** @brief Free an interface object before context closure.
    @param iface The interface object
*/
OGVR_CLIENTKIT_EXPORT OGVR_ReturnCode
    ogvrClientFreeInterface(OGVR_ClientInterface iface);

/** @} */
OGVR_EXTERN_C_END

#endif
