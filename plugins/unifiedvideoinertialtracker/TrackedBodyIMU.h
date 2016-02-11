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

#ifndef INCLUDED_TrackedBodyIMU_h_GUID_65A86547_8C4C_43B5_906C_361178DCCE06
#define INCLUDED_TrackedBodyIMU_h_GUID_65A86547_8C4C_43B5_906C_361178DCCE06

// Internal Includes
#include "ModelTypes.h"
#include "CannedIMUMeasurement.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TimeValue.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    struct ConfigParams;
    class TrackedBody;
    class TrackedBodyIMU {
      public:
        TrackedBodyIMU(TrackedBody &body, double orientationVariance,
                       double angularVelocityVariance);

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        TrackedBody &getBody() { return m_body; }
        TrackedBody const &getBody() const { return m_body; }

        /// Processes an orientation
        CannedIMUMeasurement
        preprocessOrientation(util::time::TimeValue const &tv,
                              Eigen::Quaterniond const &quat);

        void updatePoseFromOrientation(util::time::TimeValue const &tv,
                                       Eigen::Quaterniond const &quat) {
            updatePoseFromMeasurement(tv, preprocessOrientation(tv, quat));
        }

        /// Processes an angular velocity
        CannedIMUMeasurement
        preprocessAngularVelocity(util::time::TimeValue const &tv,
                                  Eigen::Quaterniond const &deltaquat,
                                  double dt);

        void updatePoseFromAngularVelocity(util::time::TimeValue const &tv,
                                           Eigen::Quaterniond const &deltaquat,
                                           double dt) {
            updatePoseFromMeasurement(
                tv, preprocessAngularVelocity(tv, deltaquat, dt));
        }

        /// @return false if you pass a completely invalid/empty canned
        /// measurement.
        bool updatePoseFromMeasurement(util::time::TimeValue const &tv,
                                       CannedIMUMeasurement const &meas);

        bool hasPoseEstimate() const { return m_hasOrientation; }
        osvr::util::time::TimeValue const &getLastUpdate() const {
            return m_last;
        }

        bool calibrationYawKnown() const { return m_yawKnown; }
        void setCalibrationYaw(util::Angle yaw) {
            m_yaw = yaw;
            m_yawKnown = true;
        }

      private:
        ConfigParams const &getParams() const;
        TrackedBody &m_body;
        bool m_yawKnown = false;
        util::Angle m_yaw;

        double m_orientationVariance;
        double m_angularVelocityVariance;

        bool m_hasOrientation = false;
        Eigen::Quaterniond m_quat;
        util::time::TimeValue m_last;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackedBodyIMU_h_GUID_65A86547_8C4C_43B5_906C_361178DCCE06
