/** @file
    @brief Header declaring device callback types

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

#ifndef INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C
#define INCLUDED_DeviceCallbackTypesC_h_GUID_46F72CEE_3327_478F_2DED_ADAAF2EC783C

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
    @{
*/
/** @brief Function type of a Device Update callback */
typedef OSVR_ReturnCode (*OSVR_DeviceUpdateCallback)(void *userData);
/** @} */

OSVR_EXTERN_C_END

#endif
