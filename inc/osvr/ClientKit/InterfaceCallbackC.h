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

#ifndef INCLUDED_InterfaceCallbacksC_h_GUID_8F16E6CB_F998_4ABC_5B6B_4FC1E4B71BC9
#define INCLUDED_InterfaceCallbacksC_h_GUID_8F16E6CB_F998_4ABC_5B6B_4FC1E4B71BC9

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientCallbackTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
    osvrRegisterPoseCallback(OSVR_ClientInterface iface, OSVR_PoseCallback cb,
                             void *userdata);

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
    osvrRegisterPositionCallback(OSVR_ClientInterface iface,
                                 OSVR_PositionCallback cb, void *userdata);

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
    osvrRegisterOrientationCallback(OSVR_ClientInterface iface,
                                    OSVR_OrientationCallback cb,
                                    void *userdata);
OSVR_EXTERN_C_END

#endif
