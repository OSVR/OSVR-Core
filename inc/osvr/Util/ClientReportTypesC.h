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

#ifndef INCLUDED_ClientReportTypesC_h_GUID_E79DAB07_78B7_4795_1EB9_CA6EEB274AEE
#define INCLUDED_ClientReportTypesC_h_GUID_E79DAB07_78B7_4795_1EB9_CA6EEB274AEE

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @brief Report type for a position callback on a tracker interface */
typedef struct OSVR_PositionReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The position vector */
    OSVR_Vec3 xyz;
} OSVR_PositionReport;

/** @brief Report type for an orientation callback on a tracker interface */
typedef struct OSVR_OrientationReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The rotation unit quaternion */
    OSVR_Quaternion rotation;
} OSVR_OrientationReport;

/** @brief Report type for a pose (position and orientation) callback on a
    tracker interface
*/
typedef struct OSVR_PoseReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The pose structure, containing a position vector and a rotation
        quaternion
    */
    OSVR_Pose3 pose;
} OSVR_PoseReport;

/** @brief Type of button state */
typedef uint8_t OSVR_ButtonState;

/** @brief Report type for a callback on a button interface */
typedef struct OSVR_ButtonReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The button state: 1 is pressed, 0 is not pressed. */
    OSVR_ButtonState state;
} OSVR_ButtonReport;

/** @brief Type of analog channel state */
typedef double OSVR_AnalogState;

/** @brief Report type for a callback on an analog interface */
typedef struct OSVR_AnalogReport {
    /** @brief Identifies the sensor/channel that the report comes from */
    int32_t sensor;
    /** @brief The analog state. */
    OSVR_AnalogState state;
} OSVR_AnalogReport;

/** @} */
OSVR_EXTERN_C_END

#endif
