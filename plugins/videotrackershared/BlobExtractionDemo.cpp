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
#include "BlobParams.h"
#include <OptionalStream.h>

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
        double minThreshold = 0;
        double maxThreshold = 255;
        double thresholdStep = 255;
    };

    void showImage(std::string const &title, cv::Mat const &img) {
        cv::namedWindow(title);
        cv::imshow(title, img);
    }

    /// Names for the indices in the hierarchy structure from findContours, so
    /// we don't have constants floating around
    enum {
        HIERARCHY_NEXT_SIBLING_CONTOUR = 0,
        HIERARCHY_PREV_SIBLING_CONTOUR = 1,
        HIERARCHY_FIRST_CHILD_CONTOUR = 2,
        HIERARCHY_PARENT_CONTOUR = 3,
    };

    template <typename F>
    std::vector<ContourType>
    getOutsidesOfConnectedComponents(cv::Mat input, F &&additionalPredicate) {
        std::vector<ContourType> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(input, contours, hierarchy, cv::RETR_CCOMP,
                         cv::CHAIN_APPROX_NONE);
        auto n = contours.size();
        std::vector<ContourType> ret;
        for (std::size_t i = 0; i < n; ++i) {
            // If this contour has no parent, then it's the outer contour of a
            // connected component
            if (hierarchy[i][HIERARCHY_PARENT_CONTOUR] < 0 &&
                std::forward<F>(additionalPredicate)(contours[i])) {
                ret.emplace_back(std::move(contours[i]));
            }
        }
        return ret;
    }

    template <typename F>
    void consumeHolesOfConnectedComponents(cv::Mat input, F &&continuation) {
        std::vector<ContourType> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(input, contours, hierarchy, cv::RETR_CCOMP,
                         cv::CHAIN_APPROX_NONE);
        auto n = contours.size();
        /// Loop through the outside connected components.
        for (std::size_t outsides = 0; outsides >= 0 && outsides < n;
             outsides = hierarchy[outsides][HIERARCHY_NEXT_SIBLING_CONTOUR]) {
            for (std::size_t idx =
                     hierarchy[outsides][HIERARCHY_FIRST_CHILD_CONTOUR];
                 idx >= 0 && idx < n;
                 idx = hierarchy[idx][HIERARCHY_NEXT_SIBLING_CONTOUR])
                /// We want all first-level children of connected components.
                std::forward<F>(continuation)(std::move(contours[idx]));
        }
    }

    void drawSubpixelPoint(cv::Mat image, cv::Point2d point,
                           cv::Scalar color = cv::Scalar(0, 0, 0),
                           double radius = 1.,
                           std::uint8_t fractionalBits = 3) {
        const auto SHIFT = std::pow(2, fractionalBits);
        auto intPoint = cv::Point(static_cast<int>(point.x * SHIFT),
                                  static_cast<int>(point.y * SHIFT));
        cv::circle(image, intPoint, static_cast<int>(radius * SHIFT), color, -1,
                   8, fractionalBits);
    }

    /// Either convert to gray from BGR, or clone.
    cv::Mat makeGray(cv::Mat const &input) {
        cv::Mat ret;
        if (input.channels() > 1) {
            cv::cvtColor(input, ret, cv::COLOR_BGR2GRAY);
        } else {
            ret = input.clone();
        }
        return ret;
    }

    /// Draw contours on a copy of the base image (converted to BGR, if
    /// required), with each contour a unique color thanks to a cheap hack with
    /// std::rand(). If you choose not to fill the contours, the center of each
    /// will have a subpixel-accurate circle plotted at it.
    ///
    /// @param baseImage Image to have the contours drawn on top of - will not
    /// be modified - just cloned.
    /// @param contours The contours you'd like drawn.
    /// @param fillContours Whether contours should be drawn filled, or just as
    /// outlines with a center point plotted (default)
    /// @param centerDotRadius Radius, in pixels, of the center dot to plot if
    /// fillContours is false.
    /// @param colorCenterDot Whether the center dots drawn if fillContours is
    /// false should match the corresponding contour color (default), or if they
    /// should all be black.
    cv::Mat drawColoredContours(cv::Mat const &baseImage,
                                std::vector<ContourType> const &contours,
                                bool fillContours = false,
                                double centerDotRadius = 1.2,
                                bool colorCenterDot = true) {
        cv::Mat highlightedContours;
        if (baseImage.depth() > 1) {
            highlightedContours = baseImage.clone();
        } else {
            cv::cvtColor(baseImage, highlightedContours, cv::COLOR_GRAY2BGR);
        }
        const cv::Scalar black(0, 0, 0);
        const std::size_t n = contours.size();
        for (std::size_t i = 0; i < n; ++i) {

            cv::Scalar color((std::rand() & 255), (std::rand() & 255),
                             (std::rand() & 255));
            cv::drawContours(highlightedContours, contours, i,
                             /*cv::Scalar(0, 0, 255)*/ color,
                             fillContours ? -1 : 1);

            if (!fillContours) {
                /// Draw a subpixel-accurate dot in the middle.
                auto data = getContourBasicDetails(contours[i]);
                drawSubpixelPoint(highlightedContours, data.center,
                                  colorCenterDot ? color : black,
                                  centerDotRadius);
            }
        }
        return highlightedContours;
    }
    void handleImage(std::string const &fn, bool pause) {
        BlobParams p;
        p.minCircularity = 0.75;

        /// Initial image loading
        std::cout << "Handling image " << fn << std::endl;
        cv::Mat color = cv::imread(fn, cv::IMREAD_COLOR);
        cv::Mat gray = makeGray(color);
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
