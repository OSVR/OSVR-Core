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
#include "BlobExtractor.h"
#include "cvUtils.h"

// Library/third-party includes
#include <opencv2/imgproc/imgproc.hpp> // for moments, boundingRect, arcLength...

// Standard includes
#include <cmath>
#include <utility>

namespace osvr {
namespace vbtracker {

    class ContourInterrogation {
      public:
        explicit ContourInterrogation(ContourType const &contour)
            : m_contour(contour), m_moms(cv::moments(m_contour)),
              m_area(m_moms.m00),
              m_center(m_moms.m10 / m_moms.m00, m_moms.m01 / m_moms.m00),
              m_rect(cv::boundingRect(m_contour)) {}

        double area() const { return m_area; }

        /// Approximation of a diameter based on assumption of circularity.
        double diameter() const { return 2 * std::sqrt(area() / CV_PI); }

        /// Gets perimeter - requires contour
        double perimeter() const { return cv::arcLength(m_contour, true); }

        /// As used by OpenCV, return value in [0, 1] - requires contour
        double circularity() const {
            auto perim = perimeter();
            return 4 * CV_PI * area() / (perim * perim);
        }
        cv::Point2d center() const { return m_center; }
        cv::Rect boundingRectangle() const { return m_rect; }

        ContourType &&moveOutContour() { return std::move(m_contour); }

      private:
        ContourType m_contour;
        cv::Moments m_moms;
        double m_area;
        cv::Point2d m_center;
        cv::Rect m_rect;
    };

    BlobData getBlobDataFromContour(ContourType const &contour) {
        ContourInterrogation contourDetails(contour);
        auto ret =
            BlobData{contourDetails.center(), contourDetails.area(),
                     contourDetails.circularity(), contourDetails.diameter(),
                     contourDetails.boundingRectangle()};
        /// because argument evaluation order is not defined.
        ret.contour = std::move(contourDetails.moveOutContour());
        return ret;
    }

    BlobBasics getContourBasicDetails(ContourType const &contour) {
        ContourInterrogation contourDetails(contour);
        return BlobBasics{contourDetails.center(), contourDetails.area(),
                          contourDetails.boundingRectangle()};
    }

    double getConvexity(ContourType const &contour, const double area) {
        ContourType hull;
        cv::convexHull(contour, hull);
        auto hullArea = cv::contourArea(hull);
        return area / hullArea;
    }

    LedMeasurementVec BasicThresholdBlobDetector::
    operator()(cv::Mat const &gray,
               cv::SimpleBlobDetector::Params const &params) {
        grayImage_ = gray.clone();
        auto thresh =
            static_cast<int>((params.minThreshold + params.maxThreshold) / 2);
        LedMeasurementVec ret;
        for (auto &contour : binarizeAndGetSolidComponents(thresh)) {
            auto data = getBlobDataFromContour(contour);
            if (params.filterByArea) {
                if (data.area < params.minArea || data.area > params.maxArea) {
                    continue;
                }
            }
            if (params.filterByCircularity) {
                if (data.circularity < params.minCircularity) {
                    continue;
                }
            }
            ret.push_back(LedMeasurement(castPointToFloat(data.center),
                                         static_cast<float>(data.diameter),
                                         grayImage_.size()));
        }
        return ret;
    }
    std::vector<ContourType>
    BasicThresholdBlobDetector::binarizeAndGetSolidComponents(int thresh) {
        cv::Mat binarized;
        cv::threshold(grayImage_, binarized, thresh, 255, cv::THRESH_BINARY);
        std::vector<ContourType> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(binarized, contours, hierarchy, CV_RETR_CCOMP,
                         CV_CHAIN_APPROX_NONE);
        auto n = contours.size();
        std::vector<ContourType> ret;
        for (std::size_t i = 0; i < n; ++i) {
            // If this contour has no first child, then it's solid (no
            // holes).
            if (hierarchy[i][2] < 0) {
                ret.emplace_back(std::move(contours[i]));
            }
        }
        return ret;
    }

    void BasicThresholdBlobDetector::makeFloodFillMask(cv::Mat const &gray) {
        floodFillMask_.create(gray.rows + 2, gray.cols + 2, CV_8UC1);
        origBoundsInFloodFill_ =
            cv::Rect(1, 1, floodFillMask_.cols - 2, floodFillMask_.rows - 2);
    }
    void BasicThresholdBlobDetector::augmentPoint(cv::Point peakCenter,
                                                  const int loDiff,
                                                  const int upDiff) {
        // Saving this now before we monkey with floodFillMask_
        cv::Mat scratchNot = ~floodFillMask_;
        cv::Mat grayClone = grayImage_.clone();
        cv::Rect filledBounds;
        auto m_area = cv::floodFill(
            grayClone, floodFillMask_, peakCenter, 255, &filledBounds, loDiff,
            upDiff, CV_FLOODFILL_MASK_ONLY | (/* connectivity 4 or 8 */ 4) |
                        (/* value to write in to mask */ 255 << 8));
        // Now floodFillMask_ contains the mask with both our point
        // and all other points so far. We need to split them by ANDing with
        // the NOT of the old flood-fill mask we saved earlier.
        cv::Mat thisPointResults = scratchNot & floodFillMask_;
        /// @todo
    }
} // namespace vbtracker
} // namespace osvr
