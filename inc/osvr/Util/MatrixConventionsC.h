/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_MatrixConventionsC_h_GUID_6FC7A4C6_E6C5_4A96_1C28_C3D21B909681
#define INCLUDED_MatrixConventionsC_h_GUID_6FC7A4C6_E6C5_4A96_1C28_C3D21B909681

/* Internal Includes */
#include <osvr/Util/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/StdInt.h>
#include <osvr/Util/Pose3C.h>
#include <osvr/Util/ReturnCodesC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @addtogroup UtilMath
@{
*/

/** @brief Type for passing matrix convention flags. */
typedef uint16_t OSVR_MatrixConventions;

/** @brief Bitmasks for testing matrix (primarily projection matrix) conventions
*/
typedef enum OSVR_MatrixMasks {
    OSVR_MATRIX_MASK_ROWMAJOR = 0x1,
    OSVR_MATRIX_MASK_ROWVECTORS = 0x2,
    OSVR_MATRIX_MASK_LHINPUT = 0x4,
    OSVR_MATRIX_MASK_UNSIGNEDZ = 0x8
} OSVR_MatrixMasks;

/** @brief Bit flags for specifying matrix options. Only one option may be
    specified per pair, with all the specified options combined with bitwise-or
    `|`.

    Most methods here that take matrix flags only obey the first grouping - the
    memory ordering flags. The remaining flags are primarily for use with
    projection matrix generation.
*/
typedef enum OSVR_MatrixFlags {
    /** @name Memory Order
        @{
    */
    OSVR_MATRIX_COLMAJOR = 0x0,
    OSVR_MATRIX_ROWMAJOR = OSVR_MATRIX_MASK_ROWMAJOR,
    /** @} */

    /** @name Whether matrix transforms row vectors or column vectors
        @{
    */
    OSVR_MATRIX_COLVECTORS = 0x0,
    OSVR_MATRIX_ROWVECTORS = OSVR_MATRIX_MASK_ROWVECTORS,
    /** @} */

    /** @name Whether a projection matrix takes in vectors in a right-handed or
       left-handed coordinate system
        @{
    */
    OSVR_MATRIX_RHINPUT = 0x0,
    OSVR_MATRIX_LHINPUT = OSVR_MATRIX_MASK_LHINPUT,
    /** @} */

    /** @name Whether a projection matrix outputs the near and far planes mapped
       to signed Z values (range [-1, 1]) or unsigned (range [0, 1])
        @{
    */
    OSVR_MATRIX_SIGNEDZ = 0x0,
    OSVR_MATRIX_UNSIGNEDZ = OSVR_MATRIX_MASK_UNSIGNEDZ
    /** @} */

} OSVR_MatrixFlags;

/** @brief Constant for the number of elements in the matrices we use - 4x4. */
enum { OSVR_MATRIX_SIZE = 16 };

/** @brief Set a matrix of doubles based on a Pose3.
    @param pose The Pose3 to convert
    @param flags Memory ordering flag
    @param mat Output: an array of 16 doubles
*/
OSVR_UTIL_EXPORT OSVR_ReturnCode osvrPose3ToMatrixd(
    OSVR_Pose3 const *pose, OSVR_MatrixConventions flags, double *mat);

/** @brief Set a matrix of floats based on a Pose3.
    @param pose The Pose3 to convert
    @param flags Memory ordering flag
    @param mat Output: an array of 16 floats
*/
OSVR_UTIL_EXPORT OSVR_ReturnCode osvrPose3ToMatrixf(
    OSVR_Pose3 const *pose, OSVR_MatrixConventions flags, float *mat);

OSVR_EXTERN_C_END

#ifdef __cplusplus
/** @brief Set a matrix based on a Pose3. (C++-only overload - detecting scalar
 * type) */
inline OSVR_ReturnCode osvrPose3ToMatrix(OSVR_Pose3 const *pose,
                                         OSVR_MatrixConventions flags,
                                         double *mat) {
    return osvrPose3ToMatrixd(pose, flags, mat);
}

/** @brief Set a matrix based on a Pose3. (C++-only overload - detecting scalar
 * type) */
inline OSVR_ReturnCode osvrPose3ToMatrix(OSVR_Pose3 const *pose,
                                         OSVR_MatrixConventions flags,
                                         float *mat) {
    return osvrPose3ToMatrixf(pose, flags, mat);
}

/** @brief Set a matrix based on a Pose3. (C++-only overload - detects scalar
 * and takes array rather than pointer) */
template <typename Scalar>
inline OSVR_ReturnCode osvrPose3ToMatrix(OSVR_Pose3 const *pose,
                                         OSVR_MatrixConventions flags,
                                         Scalar mat[OSVR_MATRIX_SIZE]) {
    return osvrPose3ToMatrix(pose, flags, &(mat[0]));
}
/** @brief Set a matrix based on a Pose3. (C++-only overload - detects scalar,
 * takes array, takes pose by reference) */
template <typename Scalar>
inline OSVR_ReturnCode osvrPose3ToMatrix(OSVR_Pose3 const &pose,
                                         OSVR_MatrixConventions flags,
                                         Scalar mat[OSVR_MATRIX_SIZE]) {
    return osvrPose3ToMatrix(&pose, flags, &(mat[0]));
}

#endif

/** @} */

#endif
