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
typedef struct OSVR_Vec3 {
    /** @brief Internal array data. */
    double data[3];
} OSVR_Vec3;

#define OSVR_VEC_MEMBER(COMPONENT, INDEX)                                      \
    /** @brief Accessor for Vec3 component COMPONENT */                        \
    OSVR_INLINE double osvrVec3Get##COMPONENT(OSVR_Vec3 *v) {                  \
        return v->data[INDEX];                                                 \
    }                                                                          \
    /** @brief Setter for Vec3 component COMPONENT */                          \
    OSVR_INLINE void osvrVec3Set##COMPONENT(OSVR_Vec3 *v, double val) {        \
        v->data[INDEX] = val;                                                  \
    }

OSVR_VEC_MEMBER(X, 0)
OSVR_VEC_MEMBER(Y, 1)
OSVR_VEC_MEMBER(Z, 2)

#undef OSVR_VEC_MEMBER

/** @} */

OSVR_EXTERN_C_END

#ifdef __cplusplus
template <typename StreamType>
inline StreamType &operator<<(StreamType &os, OSVR_Vec3 const &vec) {
    os << "(" << vec.data[0] << ", " << vec.data[1] << ", " << vec.data[2]
       << ")";
    return os;
}
#endif

#endif
