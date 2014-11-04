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

#ifndef INCLUDED_PluginCallbackTypesC_h_GUID_40B038A2_56DB_419D_BD23_DB1747FE2F10
#define INCLUDED_PluginCallbackTypesC_h_GUID_40B038A2_56DB_419D_BD23_DB1747FE2F10

/* Internal Includes */
#include <ogvr/Util/ReturnCodesC.h>
#include <ogvr/Util/PluginRegContextC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @brief Function type of a Plugin Data Delete callback */
typedef void (*OGVR_PluginDataDeleteCallback)(void *pluginData);

/** @brief Function type of a Hardware Poll callback */
typedef OGVR_ReturnCode (*OGVRHardwarePollCallback)(OGVR_PluginRegContext ctx,
                                                    void *userData);

OGVR_EXTERN_C_END

#endif
