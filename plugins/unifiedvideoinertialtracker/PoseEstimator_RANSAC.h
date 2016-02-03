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

#ifndef INCLUDED_PoseEstimator_RANSAC_h_GUID_8EB5CE6C_444D_40B0_80CB_CCA27F9008CE
#define INCLUDED_PoseEstimator_RANSAC_h_GUID_8EB5CE6C_444D_40B0_80CB_CCA27F9008CE

// Internal Includes
#include "ConfigParams.h"
#include "PoseEstimatorTypes.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <cstddef>

namespace osvr {
namespace vbtracker {
    class RANSACPoseEstimator {
      public:
        RANSACPoseEstimator(ConfigParams const &params);
        bool operator()(CameraParameters const &camParams,
                        LedPtrList const &leds, BeaconStateVec &beacons,
                        BodyState &state);

      private:
        void m_setStateFromCV(cv::Mat const &tvec, cv::Mat const &rvec,
                              BodyState &state);
        const std::size_t m_requiredInliers = 4;
        const std::size_t m_permittedOutliers = 2;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_PoseEstimator_RANSAC_h_GUID_8EB5CE6C_444D_40B0_80CB_CCA27F9008CE
