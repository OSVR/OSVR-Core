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

#ifndef INCLUDED_QuatlibInteropC_h_GUID_85D92019_F0CC_419C_5F6D_F5A3134AA5D4
#define INCLUDED_QuatlibInteropC_h_GUID_85D92019_F0CC_419C_5F6D_F5A3134AA5D4

/* Internal Includes */
#include <ogvr/Util/APIBaseC.h>
#include <ogvr/Util/Pose3C.h>

/* Library/third-party includes */
#include <quat.h>

/* Standard includes */
#include <string.h>

OGVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/
inline void ogvrQuatToQuatlib(q_type dest, OGVR_Quaternion const *src) {
    dest[Q_W] = ogvrQuatGetW(src);
    dest[Q_X] = ogvrQuatGetX(src);
    dest[Q_Y] = ogvrQuatGetY(src);
    dest[Q_Z] = ogvrQuatGetZ(src);
}

inline void ogvrQuatFromQuatlib(OGVR_Quaternion *dest, q_type const src) {
    ogvrQuatSetW(dest, src[Q_W]);
    ogvrQuatSetX(dest, src[Q_X]);
    ogvrQuatSetY(dest, src[Q_Y]);
    ogvrQuatSetZ(dest, src[Q_Z]);
}

inline void ogvrVec3ToQuatlib(q_vec_type dest, OGVR_Vec3 const *src) {
    memcpy((void *)(dest), (void const *)(src->data), sizeof(double) * 3);
}

inline void ogvrVec3FromQuatlib(OGVR_Vec3 *dest, q_vec_type const src) {
    memcpy((void *)(dest->data), (void const *)(src), sizeof(double) * 3);
}

inline void ogvrPose3ToQuatlib(q_xyz_quat_type *dest, OGVR_Pose3 const *src) {
    ogvrVec3ToQuatlib(dest->xyz, &(src->translation));
    ogvrQuatToQuatlib(dest->quat, &(src->rotation));
}

inline void ogvrPose3FromQuatlib(OGVR_Pose3 *dest, q_xyz_quat_type const *src) {
    ogvrVec3FromQuatlib(&(dest->translation), src->xyz);
    ogvrQuatFromQuatlib(&(dest->rotation), src->quat);
}

/** @} */

OGVR_EXTERN_C_END
#endif
