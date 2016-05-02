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

#ifndef INCLUDED_GenericBlobExtractor_h_GUID_0EE9AD0D_FCD3_498A_EF04_19D24185ED6B
#define INCLUDED_GenericBlobExtractor_h_GUID_0EE9AD0D_FCD3_498A_EF04_19D24185ED6B

// Internal Includes
#include "LedMeasurement.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <memory>

namespace osvr {
namespace vbtracker {

    /// This is an interface/base class for blob extractors that have the
    /// ability to provide graphical debug views at some of their inner workings
    /// but otherwise can be treated roughly like black boxes.
    class GenericBlobExtractor {
      public:
        virtual ~GenericBlobExtractor();

        cv::Mat const &getLatestGrayImage() const { return lastGrayImage_; }
        cv::Mat const &getDebugThresholdImage();
        cv::Mat const &getDebugBlobImage();

        LedMeasurementVec const &extractBlobs(cv::Mat const &grayImage);
        LedMeasurementVec const &getLatestMeasurements() const {
            return latestMeasurements_;
        }

      protected:
        virtual cv::Mat generateDebugThresholdImage_() const = 0;
        virtual cv::Mat generateDebugBlobImage_() const = 0;
        virtual LedMeasurementVec extractBlobs_() = 0;
        GenericBlobExtractor() = default;

      private:
        cv::Mat lastGrayImage_;
        LedMeasurementVec latestMeasurements_;

        bool m_debugThresholdImageDirty = true;
        cv::Mat m_debugThresholdImage;

        bool m_debugBlobImageDirty = true;
        cv::Mat m_debugBlobImage;
    };
    using BlobExtractorPtr = std::shared_ptr<GenericBlobExtractor>;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_GenericBlobExtractor_h_GUID_0EE9AD0D_FCD3_498A_EF04_19D24185ED6B
