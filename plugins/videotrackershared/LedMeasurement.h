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

#ifndef INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6
#define INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6

// Internal Includes
#include "BasicTypes.h"

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace vbtracker {
    struct LedMeasurement {
        LedMeasurement() = default;
        LedMeasurement(cv::KeyPoint const &kp, cv::Size imgSize)
            : loc(kp.pt), imageSize(imgSize), brightness(kp.size),
              diameter(kp.size), area(static_cast<float>(
                                     (diameter / 2) * (diameter / 2) * CV_PI)) {
        }

        /// Location in image space - should be undistorted when passed to the
        /// Led class.
        cv::Point2f loc;

        /// Size of the image the measurement came from.
        cv::Size imageSize;

        /// "Brightness" - currently actually diameter or radius.
        Brightness brightness;

        /// Blob diameter in pixels.
        float diameter = 0.f;

        /// Area in pixels
        float area = 1.f;

        /// Blob circularity (as defined by OpenCV) - in [0,1]
        float circularity = 0.f;

        /// Do we know an upright bounding box? (that is, is the next member
        /// valid?)
        bool knowBoundingBox = false;

        /// Dimensions of the upright bounding box.
        cv::Size2f boundingBox;
    };
    typedef std::vector<LedMeasurement> LedMeasurementVec;
    typedef LedMeasurementVec::iterator LedMeasurementVecIterator;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6
