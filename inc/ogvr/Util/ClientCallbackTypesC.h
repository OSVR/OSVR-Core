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

#ifndef INCLUDED_ClientCallbackTypesC_h_GUID_4D43A675_C8A4_4BBF_516F_59E6C785E4EF
#define INCLUDED_ClientCallbackTypesC_h_GUID_4D43A675_C8A4_4BBF_516F_59E6C785E4EF

/* Internal Includes */
#include <ogvr/Util/APIBaseC.h>
#include <ogvr/Util/ReturnCodesC.h>
#include <ogvr/Util/Pose3C.h>
#include <ogvr/Util/TimeValueC.h>
#include <ogvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OGVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/
struct OGVR_PositionReport {
    int32_t sensor;
    struct OGVR_Vec3 xyz;
};

typedef void (*OGVR_PositionCallback)(void *userdata,
                                      const struct OGVR_TimeValue timestamp,
                                      const struct OGVR_PositionReport report);

struct OGVR_OrientationReport {
    int32_t sensor;
    struct OGVR_Quaternion rotation;
};
typedef void (*OGVR_OrientationCallback)(
    void *userdata, const struct OGVR_TimeValue timestamp,
    const struct OGVR_OrientationReport report);

struct OGVR_PoseReport {
    int32_t sensor;
    struct OGVR_Pose3 pose;
};
typedef void (*OGVR_PoseCallback)(void *userdata,
                                  const struct OGVR_TimeValue timestamp,
                                  const struct OGVR_PoseReport report);

/** @} */
OGVR_EXTERN_C_END

#endif
