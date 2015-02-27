/** @file
    @brief Header

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

#define OSVR_INTERFACE_CALLBACK_METHOD(TYPE)                                   \
    /** @brief Register a callback for TYPE reports on an interface */         \
    OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrRegister##TYPE##Callback(        \
        OSVR_ClientInterface iface, OSVR_##TYPE##Callback cb, void *userdata);

OSVR_INTERFACE_CALLBACK_METHOD(Pose)
OSVR_INTERFACE_CALLBACK_METHOD(Position)
OSVR_INTERFACE_CALLBACK_METHOD(Orientation)
OSVR_INTERFACE_CALLBACK_METHOD(Button)
OSVR_INTERFACE_CALLBACK_METHOD(Analog)
OSVR_INTERFACE_CALLBACK_METHOD(Imaging)

#undef OSVR_INTERFACE_CALLBACK_METHOD

OSVR_EXTERN_C_END

#endif
