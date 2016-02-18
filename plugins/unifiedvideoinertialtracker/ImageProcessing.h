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

#ifndef INCLUDED_ImageProcessing_h_GUID_3E426FCE_BED1_4DAC_0669_70D55A14A507
#define INCLUDED_ImageProcessing_h_GUID_3E426FCE_BED1_4DAC_0669_70D55A14A507

// Internal Includes
#include "LedMeasurement.h"
#include "CameraParameters.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <memory>

namespace osvr {
namespace vbtracker {
    struct ImageProcessingOutput {
        util::time::TimeValue tv;
        LedMeasurementVec ledMeasurements;
        cv::Mat frame;
        cv::Mat frameGray;
        CameraParameters camParams;
    };
    using ImageOutputDataPtr = std::unique_ptr<ImageProcessingOutput>;
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ImageProcessing_h_GUID_3E426FCE_BED1_4DAC_0669_70D55A14A507
