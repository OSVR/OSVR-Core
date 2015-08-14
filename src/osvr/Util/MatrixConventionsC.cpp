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

// Library/third-party includes
// - none

// Standard includes
// - none

static_assert(Eigen::Matrix4d::IsRowMajor == false,
              "This and related code in MatrixConventions assume Eigen is "
              "still column-major by default.");

inline static bool isRowMajor(OSVR_MatrixConventions flags) {
    return flags & OSVR_MATRIX_MASK_ROWMAJOR;
}
inline static bool isRowVectors(OSVR_MatrixConventions flags) {
    return flags & OSVR_MATRIX_MASK_ROWVECTORS;
}
inline static bool needsTranspose(OSVR_MatrixConventions flags) {
    return isRowMajor(flags) ^ isRowVectors(flags);
}
inline static bool isLHInput(OSVR_MatrixConventions flags) {
    return flags & OSVR_MATRIX_MASK_LHINPUT;
}
inline static bool isUnsignedZ(OSVR_MatrixConventions flags) {
    return flags & OSVR_MATRIX_MASK_UNSIGNEDZ;
}
static_assert((OSVR_MATRIX_MASK_ROWMAJOR | OSVR_MATRIX_MASK_ROWVECTORS |
               OSVR_MATRIX_MASK_LHINPUT | OSVR_MATRIX_MASK_UNSIGNEDZ) ==
                  (OSVR_MATRIX_MASK_ROWMAJOR ^ OSVR_MATRIX_MASK_ROWVECTORS ^
                   OSVR_MATRIX_MASK_LHINPUT ^ OSVR_MATRIX_MASK_UNSIGNEDZ),
              "Matrix convention masks must be independent!");
template <typename Scalar>
static inline OSVR_ReturnCode pose3toMatrix(Scalar *mat, OSVR_Pose3 const *pose,
                                            OSVR_MatrixConventions flags) {
    if (!mat || !pose) {
        return OSVR_RETURN_FAILURE;
    }
    auto xform = osvr::util::fromPose(*pose);
    Eigen::Map<Eigen::Matrix<Scalar, 4, 4>> target(mat);
    if (needsTranspose(flags)) {
        target = xform.matrix().cast<Scalar>().transpose();
    } else {
        target = xform.matrix().cast<Scalar>();
    }
    return OSVR_RETURN_SUCCESS;
}
OSVR_ReturnCode osvrPose3ToMatrixd(double *mat, OSVR_Pose3 const *pose,
                                   OSVR_MatrixConventions flags) {
    return pose3toMatrix(mat, pose, flags);
}

OSVR_ReturnCode osvrPose3ToMatrixf(float *mat, OSVR_Pose3 const *pose,
                                   OSVR_MatrixConventions flags) {
    return pose3toMatrix(mat, pose, flags);
}

namespace {
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
}
