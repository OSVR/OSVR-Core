/** @file
    @brief Header declaring a type and values for simple C return codes.

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

#ifndef INCLUDED_ReturnCodesC_h_GUID_C81A2FDE_E5BB_4AAA_70A4_C616DD7C141A
#define INCLUDED_ReturnCodesC_h_GUID_C81A2FDE_E5BB_4AAA_70A4_C616DD7C141A

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/AnnotationMacrosC.h>

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
    @{
*/
/** @name Return Codes
    @{
*/
/** @brief The "success" value for an OSVR_ReturnCode */
#define OSVR_RETURN_SUCCESS (0)
/** @brief The "failure" value for an OSVR_ReturnCode */
#define OSVR_RETURN_FAILURE (1)
/** @brief Return type from C API OSVR functions. */
typedef OSVR_RETURN_SUCCESS_CONDITION(
    return == OSVR_RETURN_SUCCESS) char OSVR_ReturnCode;
/** @} */

/** @} */ /* end of group */

OSVR_EXTERN_C_END

#endif
