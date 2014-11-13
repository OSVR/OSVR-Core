/** @file
    @brief Header declaring opaque types used by @ref Client and @ref ClientKit

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

#ifndef INCLUDED_ClientOpaqueTypesC_h_GUID_24B79ED2_5751_4BA2_1690_BBD250EBC0C1
#define INCLUDED_ClientOpaqueTypesC_h_GUID_24B79ED2_5751_4BA2_1690_BBD250EBC0C1

/* Internal Includes */
#include <ogvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/
/** @brief Opaque handle that should be retained by your application. You need
    only and exactly one. */
typedef struct OGVR_ClientContextObject *OGVR_ClientContext;

/** @brief Opaque handle that should be retained by your application. You need
only and exactly one. */
typedef struct OGVR_ClientInterfaceObject *OGVR_ClientInterface;

/** @} */

OGVR_EXTERN_C_END

#endif
