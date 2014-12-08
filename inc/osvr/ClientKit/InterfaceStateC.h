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

#ifndef INCLUDED_InterfaceStateC_h_GUID_8F85D178_74B9_4AA9_4E9E_243089411408
#define INCLUDED_InterfaceStateC_h_GUID_8F85D178_74B9_4AA9_4E9E_243089411408

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/AnnotationMacrosC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/TimeValueC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    /** @brief Get TYPE state from an interface, returning failure if none     \
     * exists */                                                               \
    OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrGet##TYPE##State(                \
        OSVR_ClientInterface iface, struct OSVR_TimeValue *timestamp,          \
        OSVR_##TYPE##State *state);

OSVR_CALLBACK_METHODS(Pose)
OSVR_CALLBACK_METHODS(Position)
OSVR_CALLBACK_METHODS(Orientation)
OSVR_CALLBACK_METHODS(Button)
OSVR_CALLBACK_METHODS(Analog)

#undef OSVR_CALLBACK_METHODS

OSVR_EXTERN_C_END

#endif
