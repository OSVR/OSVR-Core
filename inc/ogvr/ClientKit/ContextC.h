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
#include <ogvr/Util/ClientOpaqueTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @brief Initialize the library.

    @param applicationIdentifier A null terminated string identifying your
   application. Reverse DNS format strongly suggested.
    @param flags initialization options (reserved) - pass 0 for now.

    @returns Client context - will be needed for subsequent calls
*/
OGVR_CLIENTKIT_EXPORT OGVR_ClientContext
    ogvrClientInit(const char applicationIdentifier[],
                   uint32_t flags OGVR_CPP_ONLY(= 0));

/** @brief Updates the state of the context - call regularly in your mainloop.

    @param ctx Client context
*/
OGVR_CLIENTKIT_EXPORT OGVR_ReturnCode ogvrClientUpdate(OGVR_ClientContext ctx);

/** @brief Shutdown the library.
    @param ctx Client context
*/
OGVR_CLIENTKIT_EXPORT OGVR_ReturnCode
    ogvrClientShutdown(OGVR_ClientContext ctx);

/** @} */
OGVR_EXTERN_C_END

#endif
