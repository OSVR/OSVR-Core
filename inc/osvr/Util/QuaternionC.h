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

#ifndef INCLUDED_QuaternionC_h_GUID_1470A5FE_8209_41A6_C19E_46077FDF9C66
#define INCLUDED_QuaternionC_h_GUID_1470A5FE_8209_41A6_C19E_46077FDF9C66

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/
/** @brief A structure defining a quaternion, often a unit quaternion
 * representing 3D rotation.
*/
typedef struct OSVR_Quaternion {
    /** @brief Internal data - direct access not recommended */
    double data[4];
} OSVR_Quaternion;

#define OSVR_QUAT_MEMBER(COMPONENT, INDEX)                                     \
    /** @brief Accessor for quaternion component COMPONENT */                  \
    inline double osvrQuatGet##COMPONENT(OSVR_Quaternion const *q) {           \
        return q->data[INDEX];                                                 \
    }                                                                          \
    /** @brief Setter for quaternion component COMPONENT */                    \
    inline void osvrQuatSet##COMPONENT(OSVR_Quaternion *q, double val) {       \
        q->data[INDEX] = val;                                                  \
    }

OSVR_QUAT_MEMBER(W, 0)
OSVR_QUAT_MEMBER(X, 1)
OSVR_QUAT_MEMBER(Y, 2)
OSVR_QUAT_MEMBER(Z, 3)

#undef OSVR_QUAT_MEMBER

/** @} */

OSVR_EXTERN_C_END

#endif
