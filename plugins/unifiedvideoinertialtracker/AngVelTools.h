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

#ifndef INCLUDED_AngVelTools_h_GUID_628878EF_DAFC_4486_36BC_6C47BD452AB0
#define INCLUDED_AngVelTools_h_GUID_628878EF_DAFC_4486_36BC_6C47BD452AB0

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenQuatExponentialMap.h>

// Standard includes
#include <cmath>

namespace osvr {
namespace vbtracker {
    /// use only for derivatives - has factor of 2/0.5 in it!
    inline Eigen::Quaterniond
    angVelVecToIncRot(Eigen::Vector3d const &angVelVec, double dt) {
        return util::quat_exp(angVelVec * dt * 0.5).normalized();
    }

    /// use only for derivatives - has factor of 2/0.5 in it!
    inline Eigen::Vector3d incRotToAngVelVec(Eigen::Quaterniond const &incRot,
                                             double dt) {
#if 0
        if (incRot.w() >= 1. || incRot.vec().isZero(1e-10)) {
            return Eigen::Vector3d::Zero();
        }
        auto angle = std::acos(incRot.w());
        return incRot.vec().normalized() * angle * 2. / dt;
#else
        return util::quat_ln(incRot) * 2. / dt;
#endif
    }

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_AngVelTools_h_GUID_628878EF_DAFC_4486_36BC_6C47BD452AB0
