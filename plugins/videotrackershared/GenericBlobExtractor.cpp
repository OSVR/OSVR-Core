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
#include "GenericBlobExtractor.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    GenericBlobExtractor::~GenericBlobExtractor() {}

    cv::Mat const &GenericBlobExtractor::getDebugThresholdImage() {
        if (m_debugThresholdImageDirty) {
            m_debugThresholdImage = generateDebugThresholdImage_();
            m_debugThresholdImageDirty = false;
        }
        return m_debugThresholdImage;
    }
    cv::Mat const &GenericBlobExtractor::getDebugBlobImage() {
        if (m_debugBlobImageDirty) {
            m_debugBlobImage = generateDebugBlobImage_();
            m_debugBlobImageDirty = false;
        }
        return m_debugBlobImage;
    }

    LedMeasurementVec const &
    GenericBlobExtractor::extractBlobs(cv::Mat const &grayImage) {
        latestMeasurements_.clear();
        lastGrayImage_ = grayImage.clone();

        m_debugThresholdImageDirty = true;
        m_debugBlobImageDirty = true;
        latestMeasurements_ = extractBlobs_();
        return latestMeasurements_;
    }

} // namespace vbtracker
} // namespace osvr
