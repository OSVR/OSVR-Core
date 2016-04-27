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
#include <BlobExtractor.h>
#include <BlobParams.h>
#include <OptionalStream.h>
#include <cvUtils.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <cmath>
#include <iostream>

#define OSVR_DEBUG_CONTOUR_CONDITIONS

namespace osvr {
namespace vbtracker {

    void showImage(std::string const &title, cv::Mat const &img) {
        cv::namedWindow(title);
        cv::imshow(title, img);
    }

    void handleImage(std::string const &fn, bool pause) {
        BlobParams p;
        p.filterByCircularity = true;
        p.minCircularity = 0.75;

        /// Initial image loading
        std::cout << "Handling image " << fn << std::endl;
        cv::Mat color = cv::imread(fn, cv::IMREAD_COLOR);
        if (!color.data) {
            std::cerr << "Could not load image!" << std::endl;
            return;
        }

        cv::Mat gray;
        cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);

        if (!gray.data) {
            std::cerr << "Conversion to gray failed?" << std::endl;
            return;
        }
        // showImage("Original", gray);

        /// Basic thresholding to reduce background noise
        auto thresholdInfo = ImageThresholdInfo(ImageRangeInfo(gray), p);
        cv::Mat thresholded;
        cv::threshold(gray, thresholded, p.absoluteMinThreshold, 255,
                      cv::THRESH_TOZERO);
        showImage("Cleaned", thresholded);
        cv::imwrite(fn + ".thresh.png", thresholded);

        /// Edge detection
        cv::Mat edge;
        cv::Laplacian(thresholded, edge, CV_8U, 5);
        cv::imwrite(fn + ".edge.png", edge);
        showImage("Edges", edge);

        /// Extract beacons from the edge detection image
        std::vector<ContourType> contours;
        LedMeasurementVec measurements;

        // turn the edge detection into a binary image.
        cv::Mat edgeTemp = edge > 0;

        // The lambda ("continuation") is called with each "hole" in the edge
        // detection image, it's up to us what to do with the contour we're
        // given. We examine it for suitability as an LED, and if it passes our
        // checks, add a derived measurement to our measurement vector and the
        // contour itself to our list of contours for debugging display.
        consumeHolesOfConnectedComponents(edgeTemp, [&](ContourType &&contour) {
            auto data = getBlobDataFromContour(contour);
            auto debugStream = [] {
#ifdef OSVR_DEBUG_CONTOUR_CONDITIONS
                return outputIf(std::cout, true);
#else
				return outputIf(std::cout, false);
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
                cv::getRectSubPix(gray, cv::Size(1, 1),
                                  cv::Point2f(data.center), patch);
                auto centerPointValue = patch.at<unsigned char>(0, 0);
                if (centerPointValue < p.absoluteMinThreshold) {
                    debugStream() << "Reject based on center point value: "
                                  << int(centerPointValue) << " < "
                                  << p.absoluteMinThreshold << "\n";
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
                    gray.size(), static_cast<float>(data.area));
                newMeas.circularity = static_cast<float>(data.circularity);
                newMeas.setBoundingBox(data.bounds);

                measurements.emplace_back(std::move(newMeas));
            }
            contours.emplace_back(std::move(contour));
        });

        /// Produce a colored debug image where each accepted contour is
        /// encircled with a different color.
        cv::Mat highlightedContours = drawColoredContours(gray, contours);
        showImage("Selected contours", highlightedContours);
        cv::imwrite(fn + ".contours.png", highlightedContours);
        if (pause) {
            cv::waitKey();
        }
    }

} // namespace vbtracker
} // namespace osvr
int main(int argc, char *argv[]) {
    /// Don't stop before exiting if we've got multiple to process.
    bool pause = (argc < 3);
    for (int arg = 1; arg < argc; ++arg) {
        osvr::vbtracker::handleImage(argv[arg], true);
    }
    return 0;
}
