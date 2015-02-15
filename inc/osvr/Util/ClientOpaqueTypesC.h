/** @file
    @brief Header declaring opaque types used by @ref Client and @ref ClientKit

    Must be c-safe!

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ClientOpaqueTypesC_h_GUID_24B79ED2_5751_4BA2_1690_BBD250EBC0C1
#define INCLUDED_ClientOpaqueTypesC_h_GUID_24B79ED2_5751_4BA2_1690_BBD250EBC0C1

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/
/** @brief Opaque handle that should be retained by your application. You need
    only and exactly one.

    Created by osvrClientInit() at application start.

    You are required to clean up this handle with osvrClientShutdown().
*/
typedef struct OSVR_ClientContextObject *OSVR_ClientContext;

/** @brief Opaque handle to an interface used for registering callbacks and
   getting status.

    You are not required to clean up this handle (it will be automatically
   cleaned up when the context is), but you can if you are no longer using it,
   using osvrClientFreeInterface() to inform the context that you no longer need
   this interface.
*/
typedef struct OSVR_ClientInterfaceObject *OSVR_ClientInterface;

/** @} */

OSVR_EXTERN_C_END

#endif
