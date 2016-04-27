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

#ifndef INCLUDED_BlobExtractor_h_GUID_E11BAD60_5C84_4581_C859_6E1FEF0838F5
#define INCLUDED_BlobExtractor_h_GUID_E11BAD60_5C84_4581_C859_6E1FEF0838F5

// Internal Includes
#include <BlobParams.h>
#include <LedMeasurement.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <algorithm> // for std::max
#include <vector>

namespace osvr {
namespace vbtracker {
    using ContourType = std::vector<cv::Point2i>;

    /// Data structure with a lot of info about a contour.
    struct BlobData {
        cv::Point2d center;
        double area;
        double circularity;
        double diameter;
        cv::Rect bounds;
        ContourType contour;
    };

    /// Data structure with just a few bare facts about a contour.
    struct BlobBasics {
        cv::Point2d center;
        double area;
        cv::Rect bounds;
    };

    BlobData getBlobDataFromContour(ContourType const &contour);

    BlobBasics getContourBasicDetails(ContourType const &contour);

    /// Returns a value in [0, 1] that is the ratio of the contour's area as
    /// provided to the area of the contour's convex hull.
    double getConvexity(ContourType const &contour, const double area);

    struct ImageRangeInfo {
        explicit ImageRangeInfo(cv::Mat const &img) {
            cv::minMaxIdx(img, &minVal, &maxVal);
        }
        double minVal;
        double maxVal;
        double lerp(double alpha) const {
            return minVal + (maxVal - minVal) * alpha;
        }
    };

    struct ImageThresholdInfo {

        ImageThresholdInfo(ImageRangeInfo const &rangeInfo, BlobParams const &p)
            : minThreshold(std::max(rangeInfo.lerp(p.minThresholdAlpha),
                                    p.absoluteMinThreshold)),
              maxThreshold(
                  std::max(rangeInfo.lerp(0.8), p.absoluteMinThreshold)),
              thresholdStep((maxThreshold - minThreshold) / p.thresholdSteps) {}

        ImageThresholdInfo(cv::Mat const &img, BlobParams const &p)
            : ImageThresholdInfo(ImageRangeInfo(img), p) {}

        double minThreshold = 0;
        double maxThreshold = 255;
        double thresholdStep = 255;
    };

    class BasicThresholdBlobDetector {
      public:
        LedMeasurementVec
        operator()(cv::Mat const &gray,
                   cv::SimpleBlobDetector::Params const &params);
        std::vector<ContourType> binarizeAndGetSolidComponents(int thresh);

      private:
        void makeFloodFillMask(cv::Mat const &gray);
        void augmentPoint(cv::Point peakCenter, const int loDiff = 2,
                          const int upDiff = 2);
        cv::Mat grayImage_;
        cv::Mat floodFillMask_;
        cv::Rect origBoundsInFloodFill_;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BlobExtractor_h_GUID_E11BAD60_5C84_4581_C859_6E1FEF0838F5
