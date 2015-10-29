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

#ifndef INCLUDED_ContentsInvalid_h_GUID_E3C95D05_0BEF_4A9F_933A_35FD0B7D3745
#define INCLUDED_ContentsInvalid_h_GUID_E3C95D05_0BEF_4A9F_933A_35FD0B7D3745

// Internal Includes
#include <osvr/Kalman/FlexibleKalmanBase.h>
#include <osvr/Kalman/PoseState.h>

// Library/third-party includes
// - none

// Standard includes
// - none

inline bool contentsInvalid(double n) { return !(n == 0. || std::isnormal(n)); }

template <typename Derived>
inline bool contentsInvalid(Eigen::MatrixBase<Derived> const &v) {
    return v.unaryExpr(
                [](typename Derived::Scalar s) { return contentsInvalid(s); })
        .any();
}

inline bool
contentsInvalid(osvr::kalman::pose_externalized_rotation::State const &state) {
    return contentsInvalid(state.stateVector()) ||
           contentsInvalid(state.getQuaternion().coeffs()) ||
           contentsInvalid(state.errorCovariance());
}
#endif // INCLUDED_ContentsInvalid_h_GUID_E3C95D05_0BEF_4A9F_933A_35FD0B7D3745
