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
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

/** @brief Type of 2D location state */
typedef OSVR_Vec2 OSVR_Location2DState;

/** @brief Report type for a callback on a  2D location interface */
typedef struct OSVR_Location2DReport {
    /** @brief Identifies the sensor/channel that the report comes from */
    OSVR_ChannelCount sensor;
    /** @brief The 2D location state. */
    OSVR_Location2DState location;
} OSVR_Location2DReport;

/** @brief C function type for a callback on an location2D interface */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(Location2D);

/** @brief Type of unit directional vector in 3D with no particular origin*/
typedef OSVR_Vec3 OSVR_DirectionState;

/** @brief Report type for 3D Direction vector */
typedef struct OSVR_DirectionReport {
    /** @brief Identifies the sensor/channel that the report comes from */
    OSVR_ChannelCount sensor;
    /** @brief The 3D Direction state. */
    OSVR_DirectionState direction;
} OSVR_DirectionReport;

/** @brief C function type for a callback on an location2D interface */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(Direction);

/** @brief State for 2D location report */
typedef OSVR_Location2DState OSVR_EyeTracker2DState;

/** @brief Report type for 2D location report */
typedef struct OSVR_EyeTracker2DReport {
    bool locationValid;
    OSVR_ChannelCount sensor;
    OSVR_EyeTracker2DState state;
} OSVR_EyeTracker2DReport;

/** @brief C function type for a callback for EyeTracker2D */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(EyeTracker2D);

/** @brief State for 3D gaze report */
typedef struct OSVR_EyeTracker3DState {
    OSVR_DirectionState direction;
    OSVR_PositionState basePoint;
} OSVR_EyeTracker3DState;

/** @brief Report type for 3D gaze report */
typedef struct OSVR_EyeTracker3DReport {
    bool directionValid;
    bool basePointValid;
    OSVR_ChannelCount sensor;
    OSVR_EyeTracker3DState state;
} OSVR_EyeTracker3DReport;

/** @brief C function type for a callback for EyeTracker3D */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(EyeTracker3D);

/** @brief State for a blink event */
typedef OSVR_ButtonState OSVR_EyeTrackerBlinkState;

/** @brief Report type for a blink event */
typedef struct OSVR_EyeTrackerBlinkReport {
    bool blinkValid;
    OSVR_ChannelCount sensor;
    OSVR_EyeTrackerBlinkState state;
} OSVR_EyeTrackerBlinkReport;

/** @brief C function type for a callback for EyeTrackerBlink */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(EyeTrackerBlink);

/** @brief C function type for a callback for NavigationVelocity */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(NaviVelocity);

/** @brief C function type for a callback for NavigationPosition */
OSVR_DEFINE_CLIENT_CALLBACK_TYPE(NaviPosition);

#undef OSVR_DEFINE_CALLBACK
/** @} */
OSVR_EXTERN_C_END

#endif
