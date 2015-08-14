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
    /// @param src An Eigen 4x4 matrix (source)
    /// @param dest A pointer to a contiguous 16-element array (destination for
    /// the copy)
    /// @param flags Some matrix convention flags - only the first two (the ones
    /// that affect if a transpose is needed) are observed.
    template <typename Scalar, typename T>
    inline void matrixEigenAssign(T const &src, Scalar *dest,
                                  OSVR_MatrixConventions flags) {
        bool needsTransposeFromColMaj = detail::matrixNeedsTranspose(flags);
        // If the source is a row-major matrix, then Eigen will automatically
        // transpose to col-major, so our transpose logic is flipped.
        const bool needsTranspose = (T::IsRowMajor) ? !needsTransposeFromColMaj
                                                    : needsTransposeFromColMaj;
        Eigen::Map<Eigen::Matrix<Scalar, 4, 4>> destMat(dest);
        if (needsTransposeFromColMaj) {
            destMat = src.template cast<Scalar>().transpose();
        } else {
            destMat = src.template cast<Scalar>();
        }
    }

} // namespace util
} // namespace osvr

#endif // INCLUDED_MatrixEigenAssign_h_GUID_23E844E3_DFBB_468C_1CD9_2BD88AA51871
