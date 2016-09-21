/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_CrossProductMatrix_h_GUID_CFE0AD3E_5291_4282_A691_5FA4F93A4878
#define INCLUDED_CrossProductMatrix_h_GUID_CFE0AD3E_5291_4282_A691_5FA4F93A4878

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    template <typename Derived>
    inline Eigen::Matrix<typename Derived::Scalar, 3, 3>
    skewSymmetricCrossProductMatrix3(Eigen::MatrixBase<Derived> const &vec) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        using Scalar = typename Derived::Scalar;
        using MatrixType = Eigen::Matrix<Scalar, 3, 3>;
        MatrixType ret;
        ret << Scalar(0), -(vec[2]), vec[1], // row 0
            vec[2], Scalar(0), -(vec[0]),    // row 1
            -(vec[1]), vec[0], Scalar(0);    // row 2
        return ret;
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_CrossProductMatrix_h_GUID_CFE0AD3E_5291_4282_A691_5FA4F93A4878
