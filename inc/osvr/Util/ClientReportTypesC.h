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

#ifndef INCLUDED_ClientReportTypesC_h_GUID_E79DAB07_78B7_4795_1EB9_CA6EEB274AEE
#define INCLUDED_ClientReportTypesC_h_GUID_E79DAB07_78B7_4795_1EB9_CA6EEB274AEE

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/StdInt.h>

#include <osvr/Util/Vec2C.h>
#include <osvr/Util/Vec3C.h>
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @name State types
@{
*/
/** @brief Type of position state */
typedef OSVR_Vec3 OSVR_PositionState;

/** @brief Type of orientation state */
typedef OSVR_Quaternion OSVR_OrientationState;

/** @brief Type of pose state */
typedef OSVR_Pose3 OSVR_PoseState;

/** @brief Type of button state */
typedef uint8_t OSVR_ButtonState;

/** @brief OSVR_ButtonState value indicating "button down" */
#define OSVR_BUTTON_PRESSED (1)

/** @brief OSVR_ButtonState value indicating "button up" */
#define OSVR_BUTTON_NOT_PRESSED (0)

/** @brief Type of analog channel state */
typedef double OSVR_AnalogState;

/** @} */

/** @name Report types
    @{
*/
/** @brief Report type for a position callback on a tracker interface */
typedef struct OSVR_PositionReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The position vector */
    OSVR_PositionState xyz;
} OSVR_PositionReport;

/** @brief Report type for an orientation callback on a tracker interface */
typedef struct OSVR_OrientationReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The rotation unit quaternion */
    OSVR_OrientationState rotation;
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
    OSVR_PoseState pose;
} OSVR_PoseReport;

/** @brief Report type for a callback on a button interface */
typedef struct OSVR_ButtonReport {
    /** @brief Identifies the sensor that the report comes from */
    int32_t sensor;
    /** @brief The button state: 1 is pressed, 0 is not pressed. */
    OSVR_ButtonState state;
} OSVR_ButtonReport;

/** @brief Report type for a callback on an analog interface */
typedef struct OSVR_AnalogReport {
    /** @brief Identifies the sensor/channel that the report comes from */
    int32_t sensor;
    /** @brief The analog state. */
    OSVR_AnalogState state;
} OSVR_AnalogReport;

/** @brief Type of eye gaze position in 2D which contains users's gaze/point of regard
in normalized display coordinates (in range [0, 1] in standard OSVR coordinate system)*/
//typedef OSVR_Vec2 OSVR_EyeGazePosition2DState;

/** @brief Report type for a gaze position (2D) 
typedef struct OSVR_EyeGazePosition2DReport{
	OSVR_ChannelCount sensor;
	OSVR_EyeGazePosition2DState gazePosition2D;
} OSVR_EyeGazePosition2DReport;
*/

/** @brief Type of location within a 2D region, inn normalized display coordinates 
	(in range [0, 1] in standard OSVR coordinate system)*/
typedef OSVR_Vec2 OSVR_Location2DState;

/** @brief Report type for 2D location */
typedef struct OSVR_Location2DReport{
OSVR_ChannelCount sensor;
OSVR_Location2DState location;
} OSVR_Location2DReport;


/** @brief Type of eye gaze direction in 3D which contains 3D vector (position) containing gaze
	base point of the user's respective eye in 3D device coordinates. And contains 
	3D vector (direction vector) containing the normalized gaze direction of user's respective eye*/
typedef struct OSVR_EyeGazeDirectionState{
	OSVR_Vec3 gazePosition3D;
	OSVR_Vec3 gazeDirection;
} OSVR_EyeGazeDirectionState;

/** @brief Report type for a gaze position (3D) */
typedef struct OSVR_EyeGazeDirectionReport{
	OSVR_ChannelCount sensor;
	OSVR_EyeGazeDirectionState gazePosn;
} OSVR_EyeGazeDirectionReport;

/** @brief Type for a blink event*/
//typedef bool OSVR_EyeBlinkState;

/** @brief Report type for a blink event */
typedef struct OSVR_EyeBlinkReport{
	OSVR_ChannelCount sensor;
} OSVR_EyeBlinkReport;


typedef struct OSVR_EyeGazeDirection{
	OSVR_Vec2 gazeDirection2D;
	OSVR_Vec3 gazeDirection3D;
} OSVR_EyeGazeDirection;

typedef struct OSVR_EyeTrackerState{
	OSVR_EyeGazeDirection gaze;
} OSVR_EyeTrackerState;

typedef struct OSVR_EyeTrackerReport {
	OSVR_ChannelCount sensor;
	OSVR_EyeTrackerState state;
} OSVR_EyeTrackerReport;

/** @brief Report type for an Imaging callback (forward declaration) */
struct OSVR_ImagingReport;

/** @} */

/** @} */
OSVR_EXTERN_C_END

#endif
