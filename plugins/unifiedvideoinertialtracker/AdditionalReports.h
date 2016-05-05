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

#ifndef INCLUDED_AdditionalReports_h_GUID_40CC5604_776C_45A1_FAE6_70CABB8A630E
#define INCLUDED_AdditionalReports_h_GUID_40CC5604_776C_45A1_FAE6_70CABB8A630E

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>

// configuration section - define or undefine here.
#define OSVR_OUTPUT_CAMERA_POSE
#define OSVR_OUTPUT_IMU
#define OSVR_OUTPUT_IMU_CAM
#define OSVR_OUTPUT_HMD_CAM

// Past this point, we consume those definitions.

namespace osvr {
namespace vbtracker {
    namespace extra_outputs {
        using IndexOffset = int;
        static const IndexOffset outputIndexBase = -1;

#ifdef OSVR_OUTPUT_CAMERA_POSE
        static const bool outputCam = true;
        static const IndexOffset outputCamIndex = outputIndexBase + 1;
#else
        static const bool outputCam = false;
        static const IndexOffset outputCamIndex = outputIndexBase;
#endif

#ifdef OSVR_OUTPUT_IMU
        static const bool outputImu = true;
        static const IndexOffset outputImuIndex = outputCamIndex + 1;
#else
        static const bool outputImu = false;
        static const IndexOffset outputImuIndex = outputCamIndex;
#endif

#ifdef OSVR_OUTPUT_IMU_CAM
        static const bool outputImuCam = true;
        static const IndexOffset outputImuCamIndex = outputImuIndex + 1;
#else
        static const bool outputImuCam = false;
        static const IndexOffset outputImuCamIndex = outputImuIndex;
#endif

#ifdef OSVR_OUTPUT_HMD_CAM
        static const bool outputHMDCam = true;
        static const IndexOffset outputHMDCamIndex = outputImuCamIndex + 1;
#else
        static const bool outputHMDCam = false;
        static const IndexOffset outputHMDCamIndex = outputImuCamIndex;
#endif

        static const std::size_t numExtraOutputs =
            static_cast<std::size_t>(outputHMDCamIndex - outputIndexBase);

    } // namespace extra_outputs
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_AdditionalReports_h_GUID_40CC5604_776C_45A1_FAE6_70CABB8A630E
