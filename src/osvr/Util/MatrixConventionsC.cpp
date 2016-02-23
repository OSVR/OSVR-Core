/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Util/MatrixConventionsC.h>
#include <osvr/Util/MatrixConventions.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/MatrixEigenAssign.h>

// Library/third-party includes
// - none

// Standard includes
// - none

/// @brief Scalar-type-independent implementation.
template <typename Scalar>
static inline OSVR_ReturnCode pose3toMatrix(OSVR_Pose3 const *pose,
                                            OSVR_MatrixConventions flags,
                                            Scalar *mat) {
    if (!mat || !pose) {
        return OSVR_RETURN_FAILURE;
    }
    auto xform = osvr::util::fromPose(*pose);
    osvr::util::matrixEigenAssign(xform.matrix(), flags, mat);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrPose3ToMatrixd(OSVR_Pose3 const *pose,
                                   OSVR_MatrixConventions flags, double *mat) {
    return pose3toMatrix(pose, flags, mat);
}

OSVR_ReturnCode osvrPose3ToMatrixf(OSVR_Pose3 const *pose,
                                   OSVR_MatrixConventions flags, float *mat) {
    return pose3toMatrix(pose, flags, mat);
}

namespace {
static_assert(!static_cast<bool>(Eigen::Matrix4d::IsRowMajor),
              "This and related code in MatrixConventions assume Eigen is "
              "still column-major by default.");

static_assert((OSVR_MATRIX_MASK_ROWMAJOR | OSVR_MATRIX_MASK_ROWVECTORS |
               OSVR_MATRIX_MASK_LHINPUT | OSVR_MATRIX_MASK_UNSIGNEDZ) ==
                  (OSVR_MATRIX_MASK_ROWMAJOR ^ OSVR_MATRIX_MASK_ROWVECTORS ^
                   OSVR_MATRIX_MASK_LHINPUT ^ OSVR_MATRIX_MASK_UNSIGNEDZ),
              "Matrix convention masks must be independent!");

using namespace osvr::util::detail;
static_assert(CompactMatrixConventions::ComputeBits<
                  CompactMatrixFlags::NeedsTranspose>::value == (0x1 << 0),
              "Metafunction test");
static_assert(CompactMatrixConventions::ComputeBits<
                  CompactMatrixFlags::LeftHandInput>::value == (0x1 << 1),
              "Metafunction test");
static_assert(CompactMatrixConventions::ComputeBits<
                  CompactMatrixFlags::UnsignedZ>::value == (0x1 << 2),
              "Metafunction test");
static_assert(
    CompactMatrixConventions::ComputeBits<
        CompactMatrixFlags::NeedsTranspose, CompactMatrixFlags::LeftHandInput,
        CompactMatrixFlags::UnsignedZ>::value == (1 + 2 + 4),
    "Metafunction test");
} // namespace
