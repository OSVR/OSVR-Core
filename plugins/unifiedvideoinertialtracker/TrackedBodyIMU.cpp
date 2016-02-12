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
#include "TrackingSystem.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <cmath>

namespace osvr {
namespace vbtracker {
    TrackedBodyIMU::TrackedBodyIMU(TrackedBody &body,
                                   double orientationVariance,
                                   double angularVelocityVariance)
        : m_body(body), m_yaw(0),
          m_useOrientation(getParams().imu.useOrientation),
          m_orientationVariance(orientationVariance),
          m_useAngularVelocity(getParams().imu.useAngularVelocity),
          m_angularVelocityVariance(angularVelocityVariance) {}
    void
    TrackedBodyIMU::updatePoseFromOrientation(util::time::TimeValue const &tv,
                                              Eigen::Quaterniond const &quat) {
        if (!m_yawKnown) {
            // This needs to go to calibration instead of to our own pose.
            getBody().getSystem().calibrationHandleIMUData(getBody().getId(),
                                                           tv, quat);
            return;
        }
        // Save some local state: we do have orientation ourselves now.
        m_quat = transformRawIMUOrientation(quat);
        m_hasOrientation = true;
        m_last = tv;

        if (!m_useOrientation) {
            return;
        }
        // Can it and update the pose with it.
        updatePoseFromMeasurement(tv, preprocessOrientation(tv, quat));
    }
    void TrackedBodyIMU::updatePoseFromAngularVelocity(
        util::time::TimeValue const &tv, Eigen::Quaterniond const &deltaquat,
        double dt) {
        if (!m_yawKnown) {
            // No calibration yet, and angular velocity isn't useful there.
            return;
        }

        if (!m_useAngularVelocity) {
            return;
        }
        // Can it and update the pose with it.
        updatePoseFromMeasurement(tv,
                                  preprocessAngularVelocity(tv, deltaquat, dt));
    }

    Eigen::Quaterniond TrackedBodyIMU::transformRawIMUOrientation(
        Eigen::Quaterniond const &input) const {
        BOOST_ASSERT_MSG(
            calibrationYawKnown(),
            "transform called before calibration transform known!");
        return m_yawCorrection * input;
    }

    CannedIMUMeasurement
    TrackedBodyIMU::preprocessOrientation(util::time::TimeValue const &tv,
                                          Eigen::Quaterniond const &quat) {

        auto ret = CannedIMUMeasurement{};
        ret.setOrientation(transformRawIMUOrientation(quat),
                           Eigen::Vector3d::Constant(m_orientationVariance));
        return ret;
    }

    /// Processes an angular velocity
    CannedIMUMeasurement TrackedBodyIMU::preprocessAngularVelocity(
        util::time::TimeValue const &tv, Eigen::Quaterniond const &deltaquat,
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
            rot.x() *= -1.;
        }
        Eigen::Vector3d newRot(rot.x(), rot.z(), rot.y());
        auto ret = CannedIMUMeasurement{};
        ret.setAngVel(newRot,
                      Eigen::Vector3d::Constant(m_angularVelocityVariance));
        return ret;
    }

    bool TrackedBodyIMU::updatePoseFromMeasurement(
        util::time::TimeValue const &tv, CannedIMUMeasurement const &meas) {
        if (!meas.orientationValid() && !meas.angVelValid()) {
            return false;
        }
        getBody().incorporateNewMeasurementFromIMU(tv, meas);
        return true;
    }

    ConfigParams const &TrackedBodyIMU::getParams() const {
        return getBody().getParams();
    }
} // namespace vbtracker
} // namespace osvr
