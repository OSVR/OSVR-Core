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
#include <osvr/Util/Pose3C.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup ClientKit
    @{
*/

/** @brief defines the joints that are available in skeleton as per OSVR
 * Skeleton Spec. */
typedef enum OSVR_SkeletonJoints {
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
    OSVR_SKELETON_INDEX_DISTAL_LEFT,
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
    OSVR_SKELETON_INDEX_DISTAL_RIGHT,
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
} OSVR_SkeletonJoints;

/** @brief The integer type specifying a number of joints/bones or a joint index.
*/
typedef uint32_t OSVR_JointCount;

/** @brief There are various types of skeleton reports that allow to get
 * different skeleton joints/bones. Note, Each report can include information
 * from one skeleton sensor due to connectedness of skeleton. Refer to the
 * definition of each report below for a complete description of which
 * joints/bones are included
 */
typedef enum OSVR_SkeletonReportSizes {
    OSVR_SKELETON_REP_HEAD_ = 2,
    OSVR_SKELETON_REP_ARM = 19,
    OSVR_SKELETON_REP_LEG = 4,
    OSVR_SKELETON_REP_FOOT = 4,
    OSVR_SKELETON_REP_HAND = 16,
    OSVR_SKELETON_REP_LOA1 = 21,
    OSVR_SKELETON_REP_LOA2 = 55
} OSVR_SkeletonReportSizes;

/** @brief A state of a single skeleton joint (joint/bone) */
typedef struct OSVR_SkeletonJointState {
    /** @brief A skeleton joint ID that specifies which bone/joint it is. May
     * not be valid if joint is not part of OSVR defined canonical names */
    OSVR_JointCount joint;
    /** @brief A tracker pose state */
    OSVR_Pose3 pose;
} OSVR_SkeletonJointState;

/** @brief A state of a single skeleton joint (joint/bone) */
typedef struct OSVR_SkeletonJointReport {
    /** @brief A tracker sensor that corresponds to current joint */
    OSVR_ChannelCount sensor;
    OSVR_SkeletonJointState state;
} OSVR_SkeletonJointReport;

/** @brief A type of skeleton state Low level of Articulation LOA1 (see below
 * for definition) */
typedef struct OSVR_SkeletonTrimmedState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_LOA1];
} OSVR_SkeletonTrimmedState;

/** @brief Report for Skeleton Level of Articulation 1 (LOA1) As defined in
 * H-Anim Low Level of Articulation provides a scaled down version of skeleton
 * joints and includes the following: Head, Neck, Clavicle Left/Right
 * (Shoulders), Arm Upper Left/Right (Elbows), Arm Lower Left/Right (Elbows)
 * Hand Left/Right (Wrists), Spine 0/1/2/3 (Center spine), Pelvis, Leg Upper
 * Left/Right (Hips), Leg Lower Left/Right (Knees), Foot Left/Right
*/
typedef struct OSVR_SkeletonTrimmedReport {
    OSVR_ChannelCount sensor;
    OSVR_SkeletonTrimmedState state;
} OSVR_SkeletonTrimmedReport;

/** @brief A type of skeleton state Hgh level of Articulation LOA2 (see below
 * for definition) */
typedef struct OSVR_SkeletonWholeState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_LOA2];
} OSVR_SkeletonWholeState;

/** @brief Report for Skeleton Level of Articulation 2 (LOA2) : H-Anim defines a
 *  humanoid figure with 72 joints to have high Level of articulation however
 *  OSVR Skeleton Interface defines a total of 55 joints/bones and it includes
 *  all joints described above: Pelvis, Spine 0/1/2/3, Neck, Head, Clavicle
 *  Left/Right (Shoulders), Arm Upper Left/Right (Elbows), Arm Lower
 *  Left/Right(forearms), Hand Left/Right (Wrists), Left/Right Thumb
 *  Proximal/Medial/Distal, Left/Right Index Proximal/Medial/Distal, Left/Right
 *  Middle Proximal/Medial/Distal, Left/Right Ring Proximal/Medial/Distal,
 *  Left/Right Pinky Proximal/Medial/Distal, Leg Upper Left/Right (Hips), Leg
 *  Lower Left/Right (Knees), Foot Left/Right, Toes Left/Right
*/
typedef struct OSVR_SkeletonWholeReport {
    /** @brief A skeleton interface sensor ID */
    OSVR_ChannelCount sensor;
    /** @brief A collection of skeleton joint interface */
    OSVR_SkeletonWholeState state;
} OSVR_SkeletonWholeReport;

/** @brief A type of skeleton hand state  */
typedef struct OSVR_SkeletonHandState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_HAND];
} OSVR_SkeletonHandState;

/** @brief Report for One Hand (Left or Right)
    Each hand report includes : Hand (Wrist), Thumb/Index/Middle/Ring/Pinky
   Proximal/Medial/Distal
*/
typedef struct OSVR_SkeletonHandReport {
    /** @brief A skeleton interface sensor ID */
    OSVR_ChannelCount sensor;
    /** @brief A collection of skeleton joint reports that only contains the
     * above joints */
    OSVR_SkeletonHandState state;
} OSVR_SkeletonHandReport;

/** @brief A type of skeleton arm state  */
typedef struct OSVR_SkeletonArmState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_ARM];
} OSVR_SkeletonArmState;

/** @brief Report for a single Arm (Left or Right)
    Each hand report includes : Clavicle, Arm Upper, Arm Lower, Hand,
   Thumb/Index/Middle/Ring/Pinky Proximal/Medial/Distal
*/
typedef struct OSVR_SkeletonArmReport {
    /** @brief A skeleton interface sensor ID */
    OSVR_ChannelCount sensor;
    /** @brief A collection of skeleton joint reports that only contains the
     * above joints */
    OSVR_SkeletonArmState state;
} OSVR_SkeletonArmReport;

/** @brief A type of skeleton foot state  */
typedef struct OSVR_SkeletonFootState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_FOOT];
} OSVR_SkeletonFootState;

/** @brief Report for a single foot
    The report includes the following joints: Foot, Toes
*/
typedef struct OSVR_SkeletonFootReport {
    /** @brief A skeleton interface sensor ID */
    OSVR_ChannelCount sensor;
    /** @brief A collection of skeleton joint reports that only contains the
     * above joints */
    OSVR_SkeletonFootState state;
} OSVR_SkeletonFootReport;

/** @brief A type of skeleton leg state  */
typedef struct OSVR_SkeletonLegState {
    /** @brief a collection of skeleton joint reports */
    OSVR_SkeletonJointReport joints[OSVR_SKELETON_REP_LEG];
} OSVR_SkeletonLegState;

/** @brief Report for a single leg
    The report includes the following joints: Leg Lower, Leg Upper, Foot, Toes
*/
typedef struct OSVR_SkeletonLegReport {
    /** @brief A skeleton interface sensor ID */
    OSVR_ChannelCount sensor;
    /** @brief A collection of skeleton joint reports that only contains the
     * above joints */
    OSVR_SkeletonLegState state;
} OSVR_SkeletonLegReport;

/** @} */

OSVR_EXTERN_C_END

#endif // INCLUDED_SkeletonReportTypesC_h_GUID_EDB2939D_09E4_4F84_5E98_5351B8138C5F
