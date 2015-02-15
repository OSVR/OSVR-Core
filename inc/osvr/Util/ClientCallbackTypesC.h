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

#define OSVR_DEFINE_CLIENT_CALLBACK_TYPE(TYPE)                                 \
    typedef void (*OSVR_##TYPE##Callback)(                                     \
        void *userdata, const struct OSVR_TimeValue *timestamp,                \
        const struct OSVR_##TYPE##Report *report)

/** @brief Report type for a position callback on a tracker interface */
typedef struct OSVR_PositionReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The position vector */
    struct OSVR_Vec3 xyz;
} OSVR_PositionReport;

/** @brief C function type for a position callback on a tracker interface */
typedef void (*OSVR_PositionCallback)(void *userdata,
                                      const struct OSVR_TimeValue *timestamp,
                                      const struct OSVR_PositionReport *report);

/** @brief Report type for an orientation callback on a tracker interface */
typedef struct OSVR_OrientationReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The rotation unit quaternion */
    struct OSVR_Quaternion rotation;
} OSVR_OrientationReport;

/** @brief C function type for an orientation callback on a tracker interface */
typedef void (*OSVR_OrientationCallback)(
    void *userdata, const struct OSVR_TimeValue *timestamp,
    const struct OSVR_OrientationReport *report);

/** @brief Report type for a pose (position and orientation) callback on a
    tracker interface
*/
typedef struct OSVR_PoseReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The pose structure, containing a position vector and a rotation
        quaternion
    */
    struct OSVR_Pose3 pose;
} OSVR_PoseReport;

/** @brief C function type for a pose (position and orientation) callback on a
    tracker interface
*/
typedef void (*OSVR_PoseCallback)(void *userdata,
                                  const struct OSVR_TimeValue *timestamp,
                                  const struct OSVR_PoseReport *report);

/** @brief Type of button state */
typedef uint8_t OSVR_ButtonState;

/** @brief Report type for a callback on a button interface */
typedef struct OSVR_ButtonReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The button state: 1 is pressed, 0 is not pressed. */
    OSVR_ButtonState state;
} OSVR_ButtonReport;

/** @brief C function type for a callback on a button interface
*/
typedef void (*OSVR_ButtonCallback)(void *userdata,
                                    const struct OSVR_TimeValue *timestamp,
                                    const struct OSVR_ButtonReport *report);

/** @brief Type of analog channel state */
typedef double OSVR_AnalogState;

/** @brief Report type for a callback on an analog interface */
typedef struct OSVR_AnalogReport {
    /** @brief Identifies the sensor/channel that the report comes from */
    int32_t sensor;
    /** @brief The analog state. */
    OSVR_AnalogState state;
} OSVR_AnalogReport;

/** @brief C function type for a callback on an analog interface */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(Analog);

#undef OSVR_DEFINE_CALLBACK
/** @} */
OSVR_EXTERN_C_END

#endif
