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

#ifndef INCLUDED_PluginRegContextC_h_GUID_D52319F2_F80B_46B8_EB90_0575A24B0134
#define INCLUDED_PluginRegContextC_h_GUID_D52319F2_F80B_46B8_EB90_0575A24B0134

/* Internal Includes */
#include <ogvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @brief A context pointer passed in to your plugin's entry point
    @ingroup plugin_c_api
*/
typedef void *OGVR_PluginRegContext;

OGVR_EXTERN_C_END

#endif
