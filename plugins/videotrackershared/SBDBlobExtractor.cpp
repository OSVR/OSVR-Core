/** @file
    @brief Implementation

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

// Internal Includes
#include "SBDBlobExtractor.h"

// Library/third-party includes
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

// Standard includes
// - none

#include <iostream>

namespace osvr {
namespace vbtracker {
#if 0
    /// This class used to be the "keypoint enhancer" - it now is used to
    /// after-the-fact extract additional data per keypoint.
    class KeypointDetailer {
      public:
        using ContourType = std::vector<cv::Point2i>;
        LedMeasurementVec
        augmentKeypoints(cv::Mat const &grayImage,
                         std::vector<cv::KeyPoint> const &foundKeyPoints) {

            LedMeasurementVec ret;
            cv::Mat greyCopy = grayImage.clone();

            /// Reset the flood fill mask to just have a one-pixel border on the
            /// edges.
            m_floodFillMask =
                cv::Mat::zeros(grayImage.rows + 2, grayImage.cols + 2, CV_8UC1);
            m_floodFillMask.row(0) = 1;
            m_floodFillMask.row(m_floodFillMask.rows - 1) = 1;
            m_floodFillMask.col(0) = 1;
            m_floodFillMask.col(m_floodFillMask.cols - 1) = 1;
            m_origBounds = cv::Rect(1, 1, m_floodFillMask.cols - 2,
                                    m_floodFillMask.rows - 2);
            for (auto const &keypoint : foundKeyPoints) {
                ret.push_back(augmentKeypoint(greyCopy, keypoint));
            }

            return ret;
        }

        cv::Mat getDebugImage() { return m_floodFillMask(m_origBounds); }

      private:
        LedMeasurement augmentKeypoint(cv::Mat grayImage,
                                       cv::KeyPoint origKeypoint) {
            // Saving this now before we monkey with m_floodFillMask
            cv::bitwise_not(m_floodFillMask, m_scratchNotMask);

            const int loDiff = 2;
            const int upDiff = 2;
            auto m_area = cv::floodFill(
                grayImage, m_floodFillMask, origKeypoint.pt, 255,
                &m_filledBounds, loDiff, upDiff,
                CV_FLOODFILL_MASK_ONLY | (/* connectivity 4 or 8 */ 4) |
                    (/* value to write in to mask */ 255 << 8));
            // Now m_floodFillMask contains the mask with both our point
            // and all other points so far. We need to split them by ANDing with
            // the NOT of the old flood-fill mask we saved earlier.
            cv::bitwise_and(m_scratchNotMask, m_floodFillMask,
                            m_perPointResults);
            // OK, now we have the results for just this point in per-point
            // results

            /// Initialize return value
            auto ret = LedMeasurement{origKeypoint, grayImage.size()};
            if (m_area <= 0) {
                // strange...
                return ret;
            }
            m_binarySubImage = m_perPointResults(m_filledBounds);
            computeContour();
            m_moments = haveContour() ? cv::moments(m_contour, false)
                                      : cv::moments(m_binarySubImage, true);

// the first two we are using from the keypoint description, the latter we can't
// always get (contour-based)
#if 0
            /// Estimate diameter
            ret.diameter = diameter();
            ret.area = area();
            if (haveContour()) {
                ret.circularity = circularity();
            }
#endif
            ret.knowBoundingBox = true;
            ret.boundingBox =
                cv::Size2f(m_filledBounds.width, m_filledBounds.height);
            return ret;
        }

        /// Gets the contour of the binary sub image.
        void computeContour() {

            m_haveContour = false;
            return;
            // Nudge out one pixel each direction if we can.
            auto expandedBounds = m_filledBounds;
            if (expandedBounds.x > 0) {
                expandedBounds.x--;
                expandedBounds.width++;
            }
            if (expandedBounds.y > 0) {
                expandedBounds.y--;
                expandedBounds.height++;
            }
            if (expandedBounds.br().x < m_perPointResults.cols) {
                expandedBounds.width++;
            }
            if (expandedBounds.br().y < m_perPointResults.rows) {
                expandedBounds.height++;
            }

            cv::Mat input = m_perPointResults(expandedBounds).clone();
            std::vector<ContourType> contours;
            cv::findContours(input, contours, CV_RETR_EXTERNAL,
                             CV_CHAIN_APPROX_NONE,
                             cv::Point(-1, -1) + expandedBounds.tl());
#if 0
            if (contours.size() != 1) {
                std::cout << "Weird, we have " << contours.size()
                          << " contours!" << std::endl;
            }
#endif
            if (!contours.empty()) {
                m_contour = cv::Mat(contours[0], true);
                m_haveContour = true;
            } else {
                m_haveContour = false;
            }
        }

        bool haveContour() { return m_haveContour; }

        double area() const { return static_cast<double>(m_area); }

        /// Approximation of a diameter based on assumption of circularity.
        double diameter() const { return 2 * std::sqrt(area() / CV_PI); }

        /// Gets perimeter - requires contour
        double perimeter() const { return cv::arcLength(m_contour, true); }

        /// As used by OpenCV, return value in [0, 1] - requires contour
        double circularity() const {
            auto perim = perimeter();
            return 4 * CV_PI * area() / (perim * perim);
        }
        cv::Rect m_origBounds;
        cv::Mat m_scratchNotMask;
        cv::Mat m_floodFillMask;
        /// @name Per-keypoint data
        /// @{
        int m_area;
        cv::Mat m_perPointResults;
        cv::Mat m_binarySubImage;
        cv::Rect m_filledBounds;
        bool m_haveContour = false;
        cv::Mat m_contour;
        cv::Moments m_moments;
        /// @}
    };
#endif

    SBDBlobExtractor::SBDBlobExtractor(BlobParams const &blobParams)
        : m_params(blobParams) {
        auto &p = m_params;
        /// Set up blob params
        m_sbdParams.minDistBetweenBlobs = p.minDistBetweenBlobs;

        m_sbdParams.minArea = p.minArea; // How small can the blobs be?

        // Look for bright blobs: there is a bug in this code
        m_sbdParams.filterByColor = false;
        // m_sbdParams.blobColor = static_cast<uchar>(255);

        m_sbdParams.filterByInertia =
            false; // Do we test for non-elongated blobs?
                   // m_sbdParams.minInertiaRatio = 0.5;
                   // m_sbdParams.maxInertiaRatio = 1.0;

        m_sbdParams.filterByCircularity =
            p.filterByCircularity; // Test for circularity?
        m_sbdParams.minCircularity =
            p.minCircularity; // default is 0.8, but the edge of the
                              // case can make the blobs "weird-shaped"

        m_sbdParams.filterByConvexity =
            p.filterByConvexity; // Test for convexity?
        m_sbdParams.minConvexity = p.minConvexity;
    }

    SBDBlobExtractor::~SBDBlobExtractor() {
        /// Needed here where KeypointDetailer is defined.
    }

    LedMeasurementVec const &
    SBDBlobExtractor::extractBlobs(cv::Mat const &grayImage) {
        m_latestMeasurements.clear();
        m_lastGrayImage = grayImage.clone();
        m_debugThresholdImageDirty = true;
        m_debugBlobImageDirty = true;

        getKeypoints(grayImage);

#if 0
        // This code uses the Keypoint Detailer, but is slightly unreliable.

        /// We'll apply the threshold here first, instead of trusting the flood
        /// fill to repeatably find the same points.
        cv::Mat thresholded;
        cv::threshold(grayImage, thresholded, m_sbdParams.minThreshold, 255,
                      cv::THRESH_BINARY);

        m_latestMeasurements =
            m_keypointDetailer->augmentKeypoints(thresholded, m_keyPoints);
#endif
        cv::Size sz = grayImage.size();
        /// Use the LedMeasurement constructor to do the conversion from
        /// keypoint to measurement right now.
        m_latestMeasurements.resize(m_keyPoints.size());
        std::transform(begin(m_keyPoints), end(m_keyPoints),
                       begin(m_latestMeasurements),
                       [sz](cv::KeyPoint const &kp) {
                           return LedMeasurement{kp, sz};
                       });

        return m_latestMeasurements;
    }

    void SBDBlobExtractor::getKeypoints(cv::Mat const &grayImage) {
        m_keyPoints.clear();
        //================================================================
        // Tracking the points

        // Construct a blob detector and find the blobs in the image.
        double minVal, maxVal;
        cv::minMaxIdx(grayImage, &minVal, &maxVal);
        auto &p = m_params;
        if (maxVal < p.absoluteMinThreshold) {
            /// empty image, early out!
            return;
        }

        auto imageRangeLerp = [=](double alpha) {
            return minVal + (maxVal - minVal) * alpha;
        };
        // 0.3 LERP between min and max as the min threshold, but
        // don't let really dim frames confuse us.
        m_sbdParams.minThreshold = std::max(imageRangeLerp(p.minThresholdAlpha),
                                            p.absoluteMinThreshold);
        m_sbdParams.maxThreshold =
            std::max(imageRangeLerp(0.8), p.absoluteMinThreshold);
        m_sbdParams.thresholdStep =
            (m_sbdParams.maxThreshold - m_sbdParams.minThreshold) /
            p.thresholdSteps;
/// @todo: Make a different set of parameters optimized for the
/// Oculus Dk2.
/// @todo: Determine the maximum size of a trackable blob by seeing
/// when we're so close that we can't view at least four in the
/// camera.
#if CV_MAJOR_VERSION == 2
        cv::Ptr<cv::SimpleBlobDetector> detector =
            new cv::SimpleBlobDetector(m_sbdParams);
#elif CV_MAJOR_VERSION == 3
        auto detector = cv::SimpleBlobDetector::create(m_sbdParams);
#else
#error "Unrecognized OpenCV version!"
#endif
        detector->detect(grayImage, m_keyPoints);

        // @todo: Consider computing the center of mass of a dilated bounding
        // rectangle around each keypoint to produce a more precise subpixel
        // localization of each LED.  The moments() function may be helpful
        // with this.

        // @todo: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.
    }

    cv::Mat SBDBlobExtractor::generateDebugThresholdImage() const {

        // Fake the thresholded image to give an idea of what the
        // blob detector is doing.
        auto getCurrentThresh = [&](int i) {
            return i * m_sbdParams.thresholdStep + m_sbdParams.minThreshold;
        };
        cv::Mat ret;
        cv::Mat temp;
        cv::threshold(m_lastGrayImage, ret, m_sbdParams.minThreshold, 255,
                      CV_THRESH_BINARY);
        cv::Mat tempOut;
        for (int i = 1; getCurrentThresh(i) < m_sbdParams.maxThreshold; ++i) {
            auto currentThresh = getCurrentThresh(i);
            cv::threshold(m_lastGrayImage, temp, currentThresh, currentThresh,
                          CV_THRESH_BINARY);
            cv::addWeighted(ret, 0.5, temp, 0.5, 0, tempOut);
            ret = tempOut;
        }
        return ret;
    }
    cv::Mat const &SBDBlobExtractor::getDebugThresholdImage() {
        if (m_debugThresholdImageDirty) {
            m_debugThresholdImage = generateDebugThresholdImage();
            m_debugThresholdImageDirty = false;
        }
        return m_debugThresholdImage;
    }

    cv::Mat SBDBlobExtractor::generateDebugBlobImage() const {
        cv::Mat ret;
        cv::Mat tempColor;
        cv::cvtColor(m_lastGrayImage, tempColor, CV_GRAY2BGR);
        // Draw detected blobs as blue circles.
        cv::drawKeypoints(tempColor, m_keyPoints, ret, cv::Scalar(255, 0, 0),
                          cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        return ret;
    }
    cv::Mat const &SBDBlobExtractor::getDebugBlobImage() {
        if (m_debugBlobImageDirty) {
            m_debugBlobImage = generateDebugBlobImage();
            m_debugBlobImageDirty = false;
        }
        return m_debugBlobImage;
    }
#if 0
    cv::Mat const &SBDBlobExtractor::getDebugExtraImage() {

        m_extraImage = m_keypointDetailer->getDebugImage().clone();
        return m_extraImage;
    }
#endif
} // namespace vbtracker
} // namespace osvr
