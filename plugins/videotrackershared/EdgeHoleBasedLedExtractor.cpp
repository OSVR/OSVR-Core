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
#include "EdgeHoleBasedLedExtractor.h"
#include "OptionalStream.h"
#include "cvUtils.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>
#include <utility>

#undef OSVR_SKIP_BLUR

namespace osvr {
namespace vbtracker {
    static inline void showImage(std::string const &title, cv::Mat const &img,
                                 bool showImages = true) {
        if (showImages) {
            cv::namedWindow(title);
            cv::imshow(title, img);
        }
    }
    EdgeHoleBasedLedExtractor::EdgeHoleBasedLedExtractor()
        : laplacianKSize_(5), laplacianScale_(1.3), edgeDetectionBlurSize_(3),
          edgeDetectionBlurThresh_(40.) {}
    LedMeasurementVec const &EdgeHoleBasedLedExtractor::
    operator()(cv::Mat const &gray, BlobParams const &p,
               bool verboseBlobOutput) {
        reset();

        verbose_ = verboseBlobOutput;

        gray_ = gray.clone();

        /// Set up the threshold parameters
        baseThreshVal_ = p.absoluteMinThreshold;
        auto rangeInfo = ImageRangeInfo(gray_);
        if (rangeInfo.maxVal < p.absoluteMinThreshold) {
            /// Early out - empty image!
            return measurements_;
        }

        auto thresholdInfo = ImageThresholdInfo(rangeInfo, p);
        minBeaconCenterVal_ =
            static_cast<std::uint8_t>(thresholdInfo.minThreshold);

        /// Basic thresholding to reduce background noise
        cv::threshold(gray_, thresh_, baseThreshVal_, 255, cv::THRESH_TOZERO);

        /// Edge detection
        cv::Laplacian(thresh_, edge_, CV_8U, laplacianKSize_, laplacianScale_);
#ifdef OSVR_SKIP_BLUR
        edgeBinary_ = edge_ > edgeDetectionBlurThresh_;
#else
        /// Extract beacons from the edge detection image
        // turn the edge detection into a binary image.
        // cv::Mat edgeTemp = edge_ > 0;
        cv::Mat edgeTemp; // = edge_.clone();
        cv::GaussianBlur(
            edge_, edgeTemp,
            cv::Size(edgeDetectionBlurSize_, edgeDetectionBlurSize_), 0, 0);
        // showImage("Blurred", edgeTemp);
        cv::threshold(edgeTemp, edgeBinary_, edgeDetectionBlurThresh_, 255,
                      cv::THRESH_BINARY);
#endif

        // The lambda ("continuation") is called with each "hole" in the edge
        // detection image, it's up to us what to do with the contour we're
        // given. We examine it for suitability as an LED, and if it passes our
        // checks, add a derived measurement to our measurement vector and the
        // contour itself to our list of contours for debugging display.
        consumeHolesOfConnectedComponents(
            edgeBinary_,
            [&](ContourType &&contour) { checkBlob(std::move(contour), p); });
        return measurements_;
    }
    void EdgeHoleBasedLedExtractor::checkBlob(ContourType &&contour,
                                              BlobParams const &p) {

        auto data = getBlobDataFromContour(contour);
        auto debugStream = [&] {
#ifdef OSVR_DEBUG_CONTOUR_CONDITIONS
            return outputIf(std::cout, true);
#else
            return outputIf(std::cout, verbose_);
#endif
        };

        debugStream() << "\nContour centered at " << data.center;
        debugStream() << " - diameter: " << data.diameter;
        debugStream() << " - area: " << data.area;
        debugStream() << " - circularity: " << data.circularity;
        debugStream() << " - bounding box size: " << data.bounds.size();
        if (data.area < p.minArea) {
            debugStream() << "Reject based on area: " << data.area << " < "
                          << p.minArea << " (not added to reject list)"
                                          "\n";

            // addToRejectedCenters(data);
            return;
        }

        {
            /// Check to see if we accidentally picked up a non-LED
            /// stuck between a few bright ones.
            cv::Mat patch;
            cv::getRectSubPix(gray_, cv::Size(1, 1), cv::Point2f(data.center),
                              patch);
            auto centerPointValue = patch.at<unsigned char>(0, 0);
            if (centerPointValue < minBeaconCenterVal_) {
                debugStream() << "Reject based on center point value: "
                              << int(centerPointValue) << " < "
                              << int(minBeaconCenterVal_)
                              << " (not added to reject list)"
                                 "\n";
                // addToRejectedCenters(data);
                return;
            }
        }

        if (p.filterByCircularity) {
            if (data.circularity < p.minCircularity) {
                debugStream()
                    << "Reject based on circularity: " << data.circularity
                    << " < " << p.minCircularity << "\n";
                addToRejectedCenters(data);
                return;
            }
        }
        if (p.filterByConvexity) {
            auto convexity = getConvexity(contour, data.area);
            debugStream() << " - convexity: " << convexity;
            if (convexity < p.minConvexity) {
                debugStream() << "Reject based on convexity: " << convexity
                              << " < " << p.minConvexity << "\n";
                addToRejectedCenters(data);

                return;
            }
        }

        debugStream() << "Accepted!\n";
        {
            auto newMeas = LedMeasurement(
                cv::Point2f(data.center), static_cast<float>(data.diameter),
                gray_.size(), static_cast<float>(data.area));
            newMeas.circularity = static_cast<float>(data.circularity);
            newMeas.setBoundingBox(data.bounds);

            measurements_.emplace_back(std::move(newMeas));
        }
        contours_.emplace_back(std::move(contour));
    }
} // namespace vbtracker
} // namespace osvr
