/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_ImagingC_h_GUID_6A9315B7_3483_42BE_6FE1_C8EF4EC59E49
#define INCLUDED_ImagingC_h_GUID_6A9315B7_3483_42BE_6FE1_C8EF4EC59E49

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ImagingReportTypesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @brief Free an image buffer returned from a callback.
    @param ctx Client context.
    @param buf Image buffer.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeImage(OSVR_ClientContext ctx, OSVR_ImageBufferElement *buf);

OSVR_EXTERN_C_END

#endif
