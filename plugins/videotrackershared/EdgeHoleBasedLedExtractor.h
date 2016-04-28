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

#ifndef INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A
#define INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A

// Internal Includes
#include <BlobExtractor.h>
#include <BlobParams.h>
#include <LedMeasurement.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <cstdint>
#include <tuple>
#include <vector>

namespace osvr {
namespace vbtracker {
    enum class RejectReason { Area, CenterPointValue, Circularity, Convexity };
    class EdgeHoleBasedLedExtractor {
      public:
        struct Params {
            Params();
            int preEdgeDetectionBlurSize;
            int laplacianKSize;
            double laplacianScale;
            bool postEdgeDetectionBlur;
            int postEdgeDetectionBlurSize;
            int postEdgeDetectionBlurThreshold;
        };
        explicit EdgeHoleBasedLedExtractor(
            Params const &extractorParams = Params());
        LedMeasurementVec const &operator()(cv::Mat const &gray,
                                            BlobParams const &p,
                                            bool verboseBlobOutput = false);
        using ContourId = std::size_t;
        // Contour ID and center.
        using RejectType = std::tuple<ContourId, RejectReason, cv::Point2d>;
        using RejectList = std::vector<RejectType>;

        void reset() {
            contours_.clear();
            measurements_.clear();
            rejectList_.clear();
            contourId_ = 0;
        }

        cv::Mat const &getInputGrayImage() const { return gray_; }
        cv::Mat const &getEdgeDetectedImage() const { return edge_; }
        cv::Mat const &getEdgeDetectedBinarizedImage() const {
            return edgeBinary_;
        }
        ContourList const &getContours() const { return contours_; }
        LedMeasurementVec const &getMeasurements() const {
            return measurements_;
        }
        RejectList const &getRejectList() const { return rejectList_; }

      private:
        void checkBlob(ContourType &&contour, BlobParams const &p);
        void addToRejectList(ContourId id, RejectReason reason,
                             BlobData const &data) {
            rejectList_.emplace_back(id, reason, data.center);
        }
        std::uint8_t minBeaconCenterVal_ = 127;
        cv::Mat gray_;
        cv::Mat edge_;
        cv::Mat edgeBinary_;
        ContourList contours_;
        LedMeasurementVec measurements_;
        RejectList rejectList_;
        bool verbose_ = false;

        ContourId contourId_ = 0;

        /// parameters
        const Params extParams_;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A
