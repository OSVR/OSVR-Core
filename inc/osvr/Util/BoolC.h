/** @file
    @brief Header providing a C-safe "bool" type, because we can't depend on
   Visual Studio providing proper C99 support in external-facing APIs.

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

#ifndef INCLUDED_BoolC_h_GUID_4F97BE90_2758_4BA5_B0FC_0CA92DEBA210
#define INCLUDED_BoolC_h_GUID_4F97BE90_2758_4BA5_B0FC_0CA92DEBA210

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @addtogroup Util
@{
*/

/** @brief A pre-C99-safe bool type. Canonical values for true and false are
 * provided. Interpretation of other values is not defined. */
typedef uint8_t OSVR_CBool;
/** @brief Canonical "true" value for OSVR_CBool */
#define OSVR_TRUE (1)
/** @brief Canonical "false" value for OSVR_CBool */
#define OSVR_FALSE (0)

/** @} */
OSVR_EXTERN_C_END

#endif
