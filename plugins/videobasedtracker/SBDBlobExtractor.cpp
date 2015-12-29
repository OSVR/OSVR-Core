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

namespace osvr {
namespace vbtracker {
    SBDBlobExtractor::SBDBlobExtractor(ConfigParams const &params)
        : m_params(params) {
        auto &p = m_params.blobParams;
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
    std::vector<LedMeasurement> const &
    SBDBlobExtractor::extractBlobs(cv::Mat const &grayImage) {
        m_latestMeasurements.clear();
        m_lastGrayImage = grayImage.clone();
        m_debugThresholdImageDirty = true;
        m_debugBlobImageDirty = true;

        getKeypoints(grayImage);
        for (auto &kp : m_keyPoints) {
            auto measurement = LedMeasurement{};
            /// @todo actually populate this with real data.
            measurement.loc = kp.pt;
            measurement.brightness = kp.size;
            measurement.diameter = 2 * kp.size;
            m_latestMeasurements.emplace_back(std::move(measurement));
        }
        return m_latestMeasurements;
    }

    void SBDBlobExtractor::getKeypoints(cv::Mat const &grayImage) {
        m_keyPoints.clear();
        //================================================================
        // Tracking the points

        // Construct a blob detector and find the blobs in the image.
        double minVal, maxVal;
        cv::minMaxIdx(grayImage, &minVal, &maxVal);
        auto &p = m_params.blobParams;
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

} // namespace vbtracker
} // namespace osvr
