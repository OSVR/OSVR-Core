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
// - none

// Standard includes
#include <iostream>
#include <utility>

namespace osvr {
namespace vbtracker {

    LedMeasurementVec const &EdgeHoleBasedLedExtractor::
    operator()(cv::Mat const &gray, BlobParams const &p,
               bool verboseBlobOutput) {
        reset();

        gray_ = gray.clone();

        /// Set up the threshold parameters
        baseThreshVal_ = p.absoluteMinThreshold;
        auto thresholdInfo = ImageThresholdInfo(gray_, p);
        minBeaconCenterVal_ =
            static_cast<std::uint8_t>(thresholdInfo.minThreshold);

        /// Basic thresholding to reduce background noise
        cv::threshold(gray_, thresh_, baseThreshVal_, 255, cv::THRESH_TOZERO);

        /// Edge detection
        cv::Laplacian(thresh_, edge_, CV_8U, LaplacianKSize);

        /// Extract beacons from the edge detection image
        // turn the edge detection into a binary image.
        cv::Mat edgeTemp = edge_ > 0;

        // The lambda ("continuation") is called with each "hole" in the edge
        // detection image, it's up to us what to do with the contour we're
        // given. We examine it for suitability as an LED, and if it passes our
        // checks, add a derived measurement to our measurement vector and the
        // contour itself to our list of contours for debugging display.
        consumeHolesOfConnectedComponents(edgeTemp, [&](ContourType &&contour) {
            checkBlob(std::move(contour), p);
        });
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
                          << p.minArea << "\n";
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
                              << int(minBeaconCenterVal_) << "\n";
                return;
            }
        }

        if (p.filterByCircularity) {
            if (data.circularity < p.minCircularity) {
                debugStream()
                    << "Reject based on circularity: " << data.circularity
                    << " < " << p.minCircularity << "\n";
                return;
            }
        }
        if (p.filterByConvexity) {
            auto convexity = getConvexity(contour, data.area);
            debugStream() << " - convexity: " << convexity;
            if (convexity < p.minConvexity) {
                debugStream() << "Reject based on convexity: " << convexity
                              << " < " << p.minConvexity << "\n";

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
