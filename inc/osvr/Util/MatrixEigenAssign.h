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

#ifndef INCLUDED_MatrixEigenAssign_h_GUID_23E844E3_DFBB_468C_1CD9_2BD88AA51871
#define INCLUDED_MatrixEigenAssign_h_GUID_23E844E3_DFBB_468C_1CD9_2BD88AA51871

// Internal Includes
#include <osvr/Util/MatrixConventions.h>
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Helper function template to assign/convert matrices as required.
    ///
    /// Handles memory ordering (transpose) and scalar type conversion
    /// automatically/based on the flags passed in.
    ///
    /// @param src An Eigen 4x4 fixed-size matrix or matrix expression (source)
    /// @param flags Some matrix convention flags - only the first two (the ones
    /// that affect if a transpose is needed) are observed.
    /// @param dest A pointer to a contiguous 16-element array (destination for
    /// the copy)
    template <typename Scalar, typename T>
    inline void matrixEigenAssign(T const &src, OSVR_MatrixConventions flags,
                                  Scalar *dest) {
        typedef Eigen::Matrix<Scalar, 4, 4> TargetType;
        static_assert(!TargetType::IsRowMajor, "This and other code depends on "
                                               "Eigen matrices being column "
                                               "major by default");

        // These should all be fixed-size for speed, they're small.
        EIGEN_STATIC_ASSERT_FIXED_SIZE(T);
        // Make sure we have a 4x4 matrix
        EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(T, 4, 4);

        const bool needsTransposeFromColMaj =
            detail::matrixNeedsTranspose(flags);
        // If the source is a row-major matrix, then Eigen will automatically
        // transpose to col-major (because that's what our "target type" is at
        // compile time), so our transpose logic is flipped.
        const bool needsTranspose = (T::IsRowMajor) ? !needsTransposeFromColMaj
                                                    : needsTransposeFromColMaj;
        Eigen::Map<TargetType> destMat(dest);
        if (needsTranspose) {
            destMat = src.template cast<Scalar>().transpose();
        } else {
            destMat = src.template cast<Scalar>();
        }
    }

} // namespace util
} // namespace osvr

#endif // INCLUDED_MatrixEigenAssign_h_GUID_23E844E3_DFBB_468C_1CD9_2BD88AA51871
