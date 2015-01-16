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

#ifndef INCLUDED_Pose3C_h_GUID_066CFCE2_229C_4194_5D2B_2602CCD5C439
#define INCLUDED_Pose3C_h_GUID_066CFCE2_229C_4194_5D2B_2602CCD5C439

/* Internal Includes */

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/Vec3C.h>
#include <osvr/Util/QuaternionC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/

/** @brief A structure defining a 3D (6DOF) rigid body pose: translation and
    rotation.
*/
typedef struct OSVR_Pose3 {
    /** @brief Position vector */
    OSVR_Vec3 translation;
    /** @brief Orientation as a unit quaternion */
    OSVR_Quaternion rotation;
} OSVR_Pose3;

/** @} */

OSVR_EXTERN_C_END

#endif
