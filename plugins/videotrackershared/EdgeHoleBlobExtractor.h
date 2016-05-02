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

#ifndef INCLUDED_EdgeHoleBlobExtractor_h_GUID_31496B1F_52D8_4BBC_A7FA_3F81E68C2A47
#define INCLUDED_EdgeHoleBlobExtractor_h_GUID_31496B1F_52D8_4BBC_A7FA_3F81E68C2A47

// Internal Includes
#include "BlobParams.h"
#include "EdgeHoleBasedLedExtractor.h"
#include "GenericBlobExtractor.h"
#include "LedMeasurement.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    class EdgeHoleBlobExtractor : public GenericBlobExtractor {
      public:
        EdgeHoleBlobExtractor(BlobParams const &blobParams,
                              EdgeHoleParams const &extParams);
        ~EdgeHoleBlobExtractor() override = default;

      protected:
        cv::Mat generateDebugThresholdImage_() const override;
        cv::Mat generateDebugBlobImage_() const override;
        LedMeasurementVec extractBlobs_() override;

      private:
        BlobParams m_params;
        EdgeHoleBasedLedExtractor m_extractor;
    };

    /// Factory for EdgeHoleBlobExtractor objects.
    BlobExtractorPtr makeEdgeHoleBlobExtractor(BlobParams const &blobParams,
                                               EdgeHoleParams const &extParams);

    /// If passing an EdgeHoleParams object, then call the EdgeHoldBlobExtractor
    /// factory.
    inline BlobExtractorPtr makeBlobExtractor(BlobParams const &blobParams,
                                              EdgeHoleParams const &extParams) {
        return makeEdgeHoleBlobExtractor(blobParams, extParams);
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_EdgeHoleBlobExtractor_h_GUID_31496B1F_52D8_4BBC_A7FA_3F81E68C2A47
