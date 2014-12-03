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

#ifndef INCLUDED_Vec3C_h_GUID_BF4E98ED_74CF_4785_DB61_109A00BA74DE
#define INCLUDED_Vec3C_h_GUID_BF4E98ED_74CF_4785_DB61_109A00BA74DE

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
/** @brief A structure defining a 3D vector, often a position/translation.
*/
struct OSVR_Vec3 {
    /** @brief Internal array data. */
    double data[3];
};

#define OSVR_VEC_MEMBER(COMPONENT, INDEX)                                      \
    /** @brief Accessor for Vec3 component COMPONENT */                        \
    inline double osvrVec3Get##COMPONENT(OSVR_Vec3 *v) {                       \
        return v->data[INDEX];                                                 \
    }                                                                          \
    /** @brief Setter for Vec3 component COMPONENT */                          \
    inline void osvrVecSet##COMPONENT(OSVR_Vec3 *v, double val) {              \
        v->data[INDEX] = val;                                                  \
    }

OSVR_VEC_MEMBER(X, 0)
OSVR_VEC_MEMBER(Y, 1)
OSVR_VEC_MEMBER(Z, 2)

#undef OSVR_VEC_MEMBER

/** @} */

OSVR_EXTERN_C_END

#endif
