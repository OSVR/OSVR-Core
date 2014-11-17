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
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/TimeValueC.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/
struct OSVR_PositionReport {
    int32_t sensor;
    struct OSVR_Vec3 xyz;
};

typedef void (*OSVR_PositionCallback)(void *userdata,
                                      const struct OSVR_TimeValue timestamp,
                                      const struct OSVR_PositionReport report);

struct OSVR_OrientationReport {
    int32_t sensor;
    struct OSVR_Quaternion rotation;
};
typedef void (*OSVR_OrientationCallback)(
    void *userdata, const struct OSVR_TimeValue timestamp,
    const struct OSVR_OrientationReport report);

struct OSVR_PoseReport {
    int32_t sensor;
    struct OSVR_Pose3 pose;
};
typedef void (*OSVR_PoseCallback)(void *userdata,
                                  const struct OSVR_TimeValue timestamp,
                                  const struct OSVR_PoseReport report);

/** @} */
OSVR_EXTERN_C_END

#endif
