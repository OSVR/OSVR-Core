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
#include <ogvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/
/** @brief A structure defining a quaternion, often a unit quaternion
 * representing 3D rotation.
*/
struct OGVR_Quaternion {
    double data[4];
};

inline struct OGVR_Quaternion ogvrQuatGetIdentity() {
    return {{1.0, 0.0, 0.0, 0.0}};
}

#define OGVR_QUAT_MEMBER(COMPONENT, INDEX)                                     \
    inline double ogvrQuatGet##COMPONENT(OGVR_Quaternion const *q) {           \
        return q->data[INDEX];                                                 \
    }                                                                          \
    inline void ogvrQuatSet##COMPONENT(OGVR_Quaternion *q, double val) {       \
        q->data[INDEX] = val;                                                  \
    }

OGVR_QUAT_MEMBER(W, 0)
OGVR_QUAT_MEMBER(X, 1)
OGVR_QUAT_MEMBER(Y, 2)
OGVR_QUAT_MEMBER(Z, 3)

#undef OGVR_QUAT_MEMBER

/** @} */

OGVR_EXTERN_C_END

#endif
