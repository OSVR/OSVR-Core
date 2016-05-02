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
#include "BlobParams.h"
#include "EdgeHoleBasedLedExtractor.h"
#include "GenericBlobExtractor.h"
#include "LedMeasurement.h"

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace vbtracker {
    class KeypointDetailer;

    /// A class performing blob-extraction duties on incoming frames.
    class SBDBlobExtractor {
      public:
        /// Construct with just BlobParams: uses simple blob detector.
        explicit SBDBlobExtractor(BlobParams const &blobParams);
        /// Construct with BlobParams and EdgeHoleBasedLedExtractor::Params:
        /// uses EdgeHoleBasedLedExtractor.
        SBDBlobExtractor(BlobParams const &blobParams,
                         EdgeHoleParams const &extParams);
        ~SBDBlobExtractor();
        LedMeasurementVec const &extractBlobs(cv::Mat const &grayImage);

        enum class Algo { SimpleBlobDetector, EdgeHoleExtractor };

        Algo getAlgo() const { return m_algo; }

        /// In the case of the EdgeHoleExtractor, this is actually the edge
        /// detection image.
        cv::Mat const &getDebugThresholdImage();

        cv::Mat const &getDebugBlobImage();
#if 0
        cv::Mat const &getDebugExtraImage();
#endif
      private:
        void getKeypoints(cv::Mat const &grayImage);
        cv::Mat generateDebugThresholdImage() const;
        cv::Mat generateDebugBlobImage() const;

        Algo m_algo;
        BlobParams m_params;
        EdgeHoleBasedLedExtractor m_extractor;
        cv::SimpleBlobDetector::Params m_sbdParams;
        LedMeasurementVec m_latestMeasurements;

        std::vector<cv::KeyPoint> m_keyPoints;

#if 0
        std::unique_ptr<KeypointDetailer> m_keypointDetailer;
#endif
        cv::Mat m_lastGrayImage;

        bool m_debugThresholdImageDirty = true;
        cv::Mat m_debugThresholdImage;

        bool m_debugBlobImageDirty = true;
        cv::Mat m_debugBlobImage;
#if 0
        cv::Mat m_extraImage;
#endif
    };

    class SBDGenericBlobExtractor : public GenericBlobExtractor {
      public:
        SBDGenericBlobExtractor(BlobParams const &blobParams);
        ~SBDGenericBlobExtractor() override = default;

      protected:
        cv::Mat generateDebugThresholdImage_() const override;
        cv::Mat generateDebugBlobImage_() const override;
        LedMeasurementVec extractBlobs_() override;

      private:
        void getKeypoints(cv::Mat const &grayImage);
        BlobParams m_params;
        std::vector<cv::KeyPoint> m_keyPoints;
        cv::SimpleBlobDetector::Params m_sbdParams;
    };

	BlobExtractorPtr makeBlobExtractor(BlobParams const &blobParams);
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_SBDBlobExtractor_h_GUID_E67E1F86_F827_48A3_5FA2_F9F241BA79AF
