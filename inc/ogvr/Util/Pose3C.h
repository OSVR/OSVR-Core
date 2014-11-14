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

#ifndef INCLUDED_Pose3C_h_GUID_066CFCE2_229C_4194_5D2B_2602CCD5C439
#define INCLUDED_Pose3C_h_GUID_066CFCE2_229C_4194_5D2B_2602CCD5C439

/* Internal Includes */

/* Internal Includes */
#include <ogvr/Util/APIBaseC.h>
#include <ogvr/Util/Vec3C.h>
#include <ogvr/Util/QuaternionC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/
/** @brief A structure defining a 3D (6DOF) rigid body pose: translation and
 * rotation.
*/
struct OGVR_Pose3 {
    OGVR_Vec3 translation;
    OGVR_Quaternion rotation;
};

/** @} */

OGVR_EXTERN_C_END

#endif
