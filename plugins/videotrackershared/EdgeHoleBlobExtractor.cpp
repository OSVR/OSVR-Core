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
#include "EdgeHoleBlobExtractor.h"
#include "cvUtils.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    EdgeHoleBlobExtractor::EdgeHoleBlobExtractor(
        BlobParams const &blobParams, EdgeHoleParams const &extParams)
        : m_params(blobParams), m_extractor(extParams) {}
    cv::Mat EdgeHoleBlobExtractor::generateDebugThresholdImage_() const {
        return m_extractor.getEdgeDetectedImage().clone();
    }

    cv::Mat EdgeHoleBlobExtractor::generateDebugBlobImage_() const {
        // Draw outlines and centers of detected LEDs in blue.
        cv::Mat gray = getLatestGrayImage();
        return drawSingleColoredContours(gray, m_extractor.getContours(),
                                         cv::Scalar(255, 0, 0));
    }

    LedMeasurementVec EdgeHoleBlobExtractor::extractBlobs_() {
        return m_extractor(getLatestGrayImage(), m_params);
    }

    BlobExtractorPtr
    makeEdgeHoleBlobExtractor(BlobParams const &blobParams,
                              EdgeHoleParams const &extParams) {
        auto extractor =
            std::make_shared<EdgeHoleBlobExtractor>(blobParams, extParams);
        return extractor;
    }

} // namespace vbtracker
} // namespace osvr