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

#ifndef INCLUDED_QuatlibInteropC_h_GUID_85D92019_F0CC_419C_5F6D_F5A3134AA5D4
#define INCLUDED_QuatlibInteropC_h_GUID_85D92019_F0CC_419C_5F6D_F5A3134AA5D4

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/Pose3C.h>

/* Library/third-party includes */
#include <quat.h>

/* Standard includes */
#include <string.h>

OSVR_EXTERN_C_BEGIN

/** @addtogroup UtilMath
    @{
*/
inline void osvrQuatToQuatlib(q_type dest, OSVR_Quaternion const *src) {
    dest[Q_W] = osvrQuatGetW(src);
    dest[Q_X] = osvrQuatGetX(src);
    dest[Q_Y] = osvrQuatGetY(src);
    dest[Q_Z] = osvrQuatGetZ(src);
}

inline void osvrQuatFromQuatlib(OSVR_Quaternion *dest, q_type const src) {
    osvrQuatSetW(dest, src[Q_W]);
    osvrQuatSetX(dest, src[Q_X]);
    osvrQuatSetY(dest, src[Q_Y]);
    osvrQuatSetZ(dest, src[Q_Z]);
}

inline void osvrVec3ToQuatlib(q_vec_type dest, OSVR_Vec3 const *src) {
    memcpy((void *)(dest), (void const *)(src->data), sizeof(double) * 3);
}

inline void osvrVec3FromQuatlib(OSVR_Vec3 *dest, q_vec_type const src) {
    memcpy((void *)(dest->data), (void const *)(src), sizeof(double) * 3);
}

inline void osvrPose3ToQuatlib(q_xyz_quat_type *dest, OSVR_Pose3 const *src) {
    osvrVec3ToQuatlib(dest->xyz, &(src->translation));
    osvrQuatToQuatlib(dest->quat, &(src->rotation));
}

inline void osvrPose3FromQuatlib(OSVR_Pose3 *dest, q_xyz_quat_type const *src) {
    osvrVec3FromQuatlib(&(dest->translation), src->xyz);
    osvrQuatFromQuatlib(&(dest->rotation), src->quat);
}

/** @} */

OSVR_EXTERN_C_END
#endif
