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
#include "PoseEstimator_RANSACKalman.h"

// Library/third-party includes
#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AbsolutePositionMeasurement.h>
#include <osvr/Kalman/FlexibleKalmanFilter.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    static const auto POSITION_VARIANCE_SCALE = 1.e-1;
    static const auto ORIENTATION_VARIANCE = 1.e0;
    bool RANSACKalmanPoseEstimator::
    operator()(EstimatorInOutParams const &p, LedPtrList const &leds,
               osvr::util::time::TimeValue const &frameTime) {

        Eigen::Vector3d xlate;
        Eigen::Quaterniond quat;
        /// Call the main pose estimation to get the vector and quat.
        {
            auto ret = m_ransac(p.camParams, leds, p.beacons, p.beaconDebug,
                                xlate, quat);
            if (!ret) {
                return false;
            }
        }

        /// If we got something, filter it in!

        if (p.startingTime != frameTime) {
            /// Predict first if appropriate.
            auto dt = util::time::duration(frameTime, p.startingTime);
            //auto dt = osvrTimeValueDurationSeconds(&frameTime, &p.startingTime);
            kalman::predict(p.state, p.processModel, dt);
        }

        /// Filter in the orientation
        {
            kalman::AbsoluteOrientationMeasurement<BodyState> meas(
                quat, Eigen::Vector3d::Constant(ORIENTATION_VARIANCE));
            kalman::correct(p.state, p.processModel, meas);
        }
        /// Filter in the orientation
        {
            /// we'll say variance goes up with distance squared.
            kalman::AbsolutePositionMeasurement<BodyState> meas(
                xlate, Eigen::Vector3d::Constant(POSITION_VARIANCE_SCALE *
                                                 xlate.z() * xlate.z()));
            kalman::correct(p.state, p.processModel, meas);
        }
        return true;
    }
} // namespace vbtracker
} // namespace osvr
