/** @file
    @brief Header declaring device callback types

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

#ifndef INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C
#define INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C

/* Internal Includes */
#include <ogvr/Util/ReturnCodesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
    @{
*/
/** @brief Function type of a Sync Device Update callback */
typedef OGVR_ReturnCode (*OGVR_SyncDeviceUpdateCallback)(void *userData);

/** @brief Function type of an Async Device Wait callback */
typedef OGVR_ReturnCode (*OGVR_AsyncDeviceWaitCallback)(void *userData);
/** @} */

OGVR_EXTERN_C_END

#endif
