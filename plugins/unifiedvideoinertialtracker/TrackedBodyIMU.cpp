/** @file
    @brief Implementation

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

// Internal Includes
#include "TrackedBodyIMU.h"
#include "TrackedBody.h"

// Library/third-party includes
// - none

// Standard includes
#include <cmath>

namespace osvr {
namespace vbtracker {
    static const double ORI_VARIANCE = 1.0E-5;
    static const double ANG_VEL_VARIANCE = 1.0E-8;
    TrackedBodyIMU::TrackedBodyIMU(TrackedBody &body)
        : m_body(body), m_yaw(0) {}

    CannedIMUMeasurement
    TrackedBodyIMU::processOrientation(util::time::TimeValue const &tv,
                                       Eigen::Quaterniond const &quat) {
        /// @todo apply transforms here

        m_hasOrientation = true;
        m_last = tv;
        m_quat = quat;

        auto ret = CannedIMUMeasurement{};
        ret.setOrientation(m_quat, Eigen::Vector3d::Constant(ORI_VARIANCE));
        return ret;
    }

    /// Processes an angular velocity
    CannedIMUMeasurement
    TrackedBodyIMU::processAngularVelocity(util::time::TimeValue const &tv,
                                           Eigen::Quaterniond const &deltaquat,
                                           double dt) {
        /// @todo handle transform for off-center velocity!

        /// @todo This has HDK-specific transforms in it!
        Eigen::Vector3d rot;
        if (deltaquat.w() >= 1. || deltaquat.vec().isZero(1e-10)) {
            rot = Eigen::Vector3d::Zero();
        } else {
            auto angle = std::acos(deltaquat.w());
            rot = deltaquat.vec().normalized() * angle * 2 / dt;
            /// @todo without transformations being applied to vel quats, this
            /// is needed.
            rot[1] *= -1.;
            rot[2] *= -1.;
        }
        auto ret = CannedIMUMeasurement{};
        ret.setAngVel(rot, Eigen::Vector3d::Constant(ANG_VEL_VARIANCE));
        return ret;
    }

    ConfigParams const &TrackedBodyIMU::getParams() const {
        return getBody().getParams();
    }
} // namespace vbtracker
} // namespace osvr
