/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_SBDBlobExtractor_h_GUID_E67E1F86_F827_48A3_5FA2_F9F241BA79AF
#define INCLUDED_SBDBlobExtractor_h_GUID_E67E1F86_F827_48A3_5FA2_F9F241BA79AF

// Internal Includes
#include "Types.h"
#include "LED.h"

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>
#include <memory>

namespace osvr {
namespace vbtracker {
    class KeypointDetailer;

    /// A class performing blob-extraction duties on incoming frames.
    class SBDBlobExtractor {
      public:
        explicit SBDBlobExtractor(ConfigParams const &params);
        ~SBDBlobExtractor();
        LedMeasurementVec const &
        extractBlobs(cv::Mat const &grayImage);

        cv::Mat const &getDebugThresholdImage();

        cv::Mat const &getDebugBlobImage();
        cv::Mat const &getDebugExtraImage();

      private:
        void getKeypoints(cv::Mat const &grayImage);
        cv::Mat generateDebugThresholdImage() const;
        cv::Mat generateDebugBlobImage() const;

        ConfigParams m_params;
        cv::SimpleBlobDetector::Params m_sbdParams;
        LedMeasurementVec m_latestMeasurements;

        std::vector<cv::KeyPoint> m_keyPoints;

        std::unique_ptr<KeypointDetailer> m_keypointDetailer;
        cv::Mat m_lastGrayImage;

        bool m_debugThresholdImageDirty = true;
        cv::Mat m_debugThresholdImage;

        bool m_debugBlobImageDirty = true;
        cv::Mat m_debugBlobImage;

        cv::Mat m_extraImage;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_SBDBlobExtractor_h_GUID_E67E1F86_F827_48A3_5FA2_F9F241BA79AF
