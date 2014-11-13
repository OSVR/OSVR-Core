/** @file
    @brief Header

    Must be c-safe!

    @todo Apply annotation macros

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

#ifndef INCLUDED_ContextC_h_GUID_3790F330_2425_4486_4C9F_20C300D7DED3
#define INCLUDED_ContextC_h_GUID_3790F330_2425_4486_4C9F_20C300D7DED3

/* Internal Includes */
#include <ogvr/ClientKit/Export.h>
#include <ogvr/Util/APIBaseC.h>
#include <ogvr/Util/ReturnCodesC.h>
#include <ogvr/Util/AnnotationMacrosC.h>
#include <ogvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @brief Opaque handle that should be retained by your application. You need
    only and exactly one. */
typedef struct OGVR_ClientContextObject *OGVR_ClientContext;

/** @brief Initialize the library.

    @param flags initialization options (reserved) - pass 0 for now.

    @returns Client context - will be needed for subsequent calls
*/
OGVR_CLIENTKIT_EXPORT OGVR_ClientContext
    ogvrClientInit(uint32_t flags OGVR_CPP_ONLY(= 0));

/** @brief Shutdown the library.
    @param ctx Client context
*/
OGVR_CLIENTKIT_EXPORT OGVR_ReturnCode
    ogvrClientShutdown(OGVR_ClientContext ctx);

OGVR_EXTERN_C_END

#endif
