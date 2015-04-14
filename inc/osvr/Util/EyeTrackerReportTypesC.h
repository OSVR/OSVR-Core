/** @file
@brief Header

Must be c-safe!

@date 2015

@author
Sensics, Inc.
<http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_EyeTrackerReportTypesC_h_GUID_7BD4C450_5D26_4090_BCA4_7305E3DCC6B7
#define INCLUDED_EyeTrackerReportTypesC_h_GUID_7BD4C450_5D26_4090_BCA4_7305E3DCC6B7


// Internal Includes
#include <osvr/Util/Vec2C.h>
#include <osvr/Util/Vec3C.h>
#include <osvr/Util/ChannelCountC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_EXTERN_C_BEGIN

typedef enum OSVR_Eye_Type {
	OSVR_EYE_LEFT = 0,
	OSVR_EYE_RIGHT = 1
} OSVR_Eye_Type;

typedef enum OSVR_Eye_Tracker_Mode {
	OSVR_MONOCULAR = 0,
	OSVR_BINOCULAR = 1
} OSVR_Eye_Tracker_Mode;

typedef struct OSVR_EyeData{
	OSVR_Vec2 gazeDirection2D;
	OSVR_Vec3 gazeDirection3D;
} OSVR_EyeData;

typedef struct OSVR_EyeTrackerReport {
	OSVR_ChannelCount sensor;
	OSVR_EyeData state;
} OSVR_EyeTrackerReport;


OSVR_EXTERN_C_END

#endif // INCLUDED_EyeTrackerReportTypesC_h_GUID_7BD4C450_5D26_4090_BCA4_7305E3DCC6B7

