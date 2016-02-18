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

#ifndef INCLUDED_PoseEstimator_RANSACKalman_h_GUID_D1F93D34_7D5C_4F93_4310_C3786103DCB8
#define INCLUDED_PoseEstimator_RANSACKalman_h_GUID_D1F93D34_7D5C_4F93_4310_C3786103DCB8

// Internal Includes
#include "ConfigParams.h"
#include "PoseEstimatorTypes.h"
#include "PoseEstimator_RANSAC.h"

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>

namespace osvr {
namespace vbtracker {
    class RANSACKalmanPoseEstimator {
      public:
        /// Perform RANSAC-based pose estimation but filter results in via an
        /// EKF to the body state.
        ///
        /// @return true if a pose was estimated.
        bool operator()(EstimatorInOutParams const &p, LedPtrList const &leds,
                        osvr::util::time::TimeValue const &frameTime);

      private:
        RANSACPoseEstimator m_ransac;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_PoseEstimator_RANSACKalman_h_GUID_D1F93D34_7D5C_4F93_4310_C3786103DCB8
