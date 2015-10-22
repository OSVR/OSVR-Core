/** @file
    @brief Header

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SkeletonReportTypesC_h_GUID_EDB2939D_09E4_4F84_5E98_5351B8138C5F
#define INCLUDED_SkeletonReportTypesC_h_GUID_EDB2939D_09E4_4F84_5E98_5351B8138C5F

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/ChannelCountC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

typedef enum OSVR_SkeletonElements{
    OSVR_SKELETON_PELVIS,
    OSVR_SKELETON_SPINE0,
    OSVR_SKELETON_SPINE1,
    OSVR_SKELETON_SPINE2,
    OSVR_SKELETON_SPINE3,
    OSVR_SKELETON_NECK,
    OSVR_SKELETON_HEAD,
    OSVR_SKELETON_CLAVICLE_LEFT,
    OSVR_SKELETON_ARM_UPPER_LEFT,
    OSVR_SKELETON_ARM_LOWER_LEFT,
    OSVR_SKELETON_HAND_LOWER_LEFT,
    /// left hand
    OSVR_SKELETON_HAND_LEFT,
    OSVR_SKELETON_THUMB_PROXIMAL_LEFT,
    OSVR_SKELETON_THUMB_MEDIAL_LEFT,
    OSVR_SKELETON_THUMB_DISTAL_LEFT,
    OSVR_SKELETON_INDEX_PROXIMAL_LEFT,
    OSVR_SKELETON_INDEX_MEDIAL_LEFT,
    OSVR_SKELETON_IDNEX_DISTAL_LEFT,
    OSVR_SKELETON_MIDDLE_PROXIMAL_LEFT,
    OSVR_SKELETON_MIDDLE_MEDIAL_LEFT,
    OSVR_SKELETON_MIDDLE_DISTAL_LEFT,
    OSVR_SKELETON_RING_PROXIMAL_LEFT,
    OSVR_SKELETON_RING_MEDIAL_LEFT,
    OSVR_SKELETON_RING_DISTAL_LEFT,
    OSVR_SKELETON_PINKY_PROXIMAL_LEFT,
    OSVR_SKELETON_PINKY_MEDIAL_LEFT,
    OSVR_SKELETON_PINKY_DISTAL_LEFT,
    /// end left hend
    OSVR_SKELETON_CLAVICLE_RIGHT,
    OSVR_SKELETON_ARM_UPPER_RIGHT,
    OSVR_SKELETON_ARM_LOWER_RIGHT,
    OSVR_SKELETON_HAND_LOWER_RIGHT,
    /// right hand
    OSVR_SKELETON_HAND_RIGHT,
    OSVR_SKELETON_THUMB_PROXIMAL_RIGHT,
    OSVR_SKELETON_THUMB_MEDIAL_RIGHT,
    OSVR_SKELETON_THUMB_DISTAL_RIGHT,
    OSVR_SKELETON_INDEX_PROXIMAL_RIGHT,
    OSVR_SKELETON_INDEX_MEDIAL_RIGHT,
    OSVR_SKELETON_IDNEX_DISTAL_RIGHT,
    OSVR_SKELETON_MIDDLE_PROXIMAL_RIGHT,
    OSVR_SKELETON_MIDDLE_MEDIAL_RIGHT,
    OSVR_SKELETON_MIDDLE_DISTAL_RIGHT,
    OSVR_SKELETON_RING_PROXIMAL_RIGHT,
    OSVR_SKELETON_RING_MEDIAL_RIGHT,
    OSVR_SKELETON_RING_DISTAL_RIGHT,
    OSVR_SKELETON_PINKY_PROXIMAL_RIGHT,
    OSVR_SKELETON_PINKY_MEDIAL_RIGHT,
    OSVR_SKELETON_PINKY_DISTAL_RIGHT,
    /// end right hand
    OSVR_SKELETON_LEG_UPPER_LEFT,
    OSVR_SKELETON_LEG_LOWER_LEFT,
    OSVR_SKELETON_FOOT_LEFT,
    OSVR_SKELETON_TOES_LEFT,
    OSVR_SKELETON_LEG_UPPER_RIGHT,
    OSVR_SKELETON_LEG_LOWER_RIGHT,
    OSVR_SKELETON_FOOT_RIGHT,
    OSVR_SKELETON_TOES_RIGHT
} OSVR_SkeletonElements;

/** @brief There are various types of skeleton reports that allow to get different skeleton joints/bones. Refer to the definition of each report below for a complete description of which bones are included */
typedef enum OSVR_SkeletonReportSizes {
    OSVR_SKELETON_REP_HEAD_ = 2,
    OSVR_SKELETON_REP_ARM = 19,
    OSVR_SKELETON_REP_ARMS = 38,
    OSVR_SKELETON_REP_LEG = 4,
    OSVR_SKELETON_REP_LEGS = 8,
    OSVR_SKELETON_REP_FOOT = 4,
    OSVR_SKELETON_REP_FEET = 2,
    OSVR_SKELETON_REP_HAND = 16,
    OSVR_SKELETON_REP_HANDS = 34,
    OSVR_SKELETON_REP_LOA1 = 15
    OSVR_SKELETON_REP_LOA2 = 53
} OSVR_SkeletonReportSizes;

/** @brief A state of a skeleton element (joint/bone) */
typedef struct OSVR_SkeletonElementReport{
    /** @brief A skeleton element ID that specifies which bone/joint it is. May not be valid if joint not */
    uint32_t element;
    /** @brief A tracker sensor that corresponds to current element */
    OSVR_ChannelCount sensor
    /** @brief A tracker interface */
    OSVR_PoseState state;
} OSVR_SkeletonElementReport;

/** @brief Report for Skeleton Level of Articulation 1 (LOA1)
    As defined in H-Anim Low Level of Articulation provides a scaled down version of skeleton joints and includes the following:
    Head, Clavicle Left/Right (Shoulders), Arm Upper Left/Right (Elbows), Hand Left/Right (Wrists), Spine1 (Center spine), Pelvis, Leg Upper Left/Right (Hips), Leg Lower Left/Right (Knees), Foot Left/Right
    
*/ 
typedef struct OSVR_SkeletonLOA1Report{
    uint32_t sensor;
    /** @brief a collection of tracker reports */
    OSVR_PoseReport states[OSVR_SKELETON_LOA1];
}


/** @brief Report for Skeleton Level of Articulation 2 (LOA2)
    H-Anim defines  a humanoid figure with 72 joints to have high Level of articulation however OSVR Skeleton Interface defines a total of 53 joints/bones and it includes all elements described above:
    Pelvis, Spine 0/1/2/3, Neck, Head, Clavicle Left/Right (Shoulders), Arm Upper Left/Right (Elbows), Arm Lower Left/Right (forearms), Hand Left/Right (Wrists), Left/Right Thumb Proximal/Medial/Distal, Left/Right Index Proximal/Medial/Distal, Left/Right Middle Proximal/Medial/Distal, Left/Right Ring Proximal/Medial/Distal, Left/Right Pinky Proximal/Medial/Distal, Leg Upper Left/Right (Hips), Leg Lower Left/Right (Knees), Foot Left/Right, Toes Left/Right
    
*/ 
typedef struct OSVR_SkeletonLOA2Report {
    /** @brief A skeleton interface sensor ID */
    uint32_t sensor;
    /** @brief A collection of tracker interface */
    OSVR_SkeletonState states[OSVR_SKELETON_REP_LOA2];
} OSVR_SkeletonLOA2Report;

/** @brief Report for Skeleton Hands (Left and Right)
    The report includes for Left/Right: Lower Hand, Hand, Thumb/Index/Middle/Ring/Pinky Proximal/Medial/Distal
*/ 
typedef struct OSVR_SkeletonHandsReport {
    /** @brief A skeleton interface sensor ID */
    uint32_t sensor;
    /** @brief A collection of skeleton element reports that only contains the above elements */
    OSVR_SkeletonElementReport states[OSVR_SKELETON_REP_HANDS];
} OSVR_SkeletonHandsReport;

/** @brief Report for One Hand (Left or Right)
    Each hand report includes : Lower Hand, Hand, Thumb/Index/Middle/Ring/Pinky Proximal/Medial/Distal
*/ 
typedef struct OSVR_SkeletonHandReport {
    /** @brief A skeleton interface sensor ID */
    uint32_t sensor;
    /** @brief A collection of skeleton element reports that only contains the above elements */
    OSVR_SkeletonElementReport states[OSVR_SKELETON_REP_HAND];
} OSVR_SkeletonHandReport;

/** @brief Report for both feet (Left and Right)
    The report includes the following elements: Left/Right Foot, Left/Right Toes
*/ 
typedef struct OSVR_SkeletonFeetReport {
    /** @brief A skeleton interface sensor ID */
    uint32_t sensor;
    /** @brief A collection of skeleton element reports that only contains the above elements */
    OSVR_SkeletonElementReport states[OSVR_SKELETON_REP_FEET];
} OSVR_SkeletonFeetReport;

/** @brief Report for both feet (Left and Right)
    The report includes the following elements: Left/Right Foot, Left/Right Toes
*/ 
typedef struct OSVR_SkeletonFeetReport {
    /** @brief A skeleton interface sensor ID */
    uint32_t sensor;
    /** @brief A collection of skeleton element reports that only contains the above elements */
    OSVR_SkeletonElementReport states[OSVR_SKELETON_REP_FEET];
} OSVR_SkeletonFeetReport;
/** @} */

OSVR_EXTERN_C_END

#endif // INCLUDED_SkeletonReportTypesC_h_GUID_EDB2939D_09E4_4F84_5E98_5351B8138C5F

