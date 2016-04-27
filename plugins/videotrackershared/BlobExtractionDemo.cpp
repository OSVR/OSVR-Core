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

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <cmath>
#include <iostream>

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

    void processImageAndEdges(std::string const &fn, cv::Mat color,
                              cv::Mat gray, cv::Mat edge, BlobParams const &p) {
        std::vector<ContourType> contours;
        LedMeasurementVec measurements;

        // turn the edge detection into a binary image.
        cv::Mat edgeTemp = edge > 0;

        auto contourContinuation = [&](ContourType &&contour) {
            auto data = getBlobDataFromContour(contour);
            if (data.area < p.minArea) {
                std::cout << "Reject based on area: " << data.area << " < "
                          << p.minArea << std::endl;
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
                    std::cout << "Reject based on center point value: "
                              << int(centerPointValue) << " < "
                              << p.absoluteMinThreshold << std::endl;
                    return;
                }
            }

            if (p.filterByCircularity) {
                if (data.circularity < p.minCircularity) {
                    std::cout
                        << "Reject based on circularity: " << data.circularity
                        << " < " << p.minCircularity << std::endl;
                    return;
                }
            }
            if (p.filterByConvexity) {
                auto convexity = getConvexity(contour, data.area);
                if (convexity < p.minConvexity) {
                    std::cout << "Reject based on convexity: " << convexity
                              << " < " << p.minConvexity << std::endl;
                    return;
                }
            }
            std::cout << "Accept contour with center at " << data.center
                      << std::endl;
            {
                auto newMeas = LedMeasurement(data.center.x, data.center.y,
                                              data.diameter, gray.size());
                newMeas.circularity = data.circularity;
                newMeas.knowBoundingBox = true;
                newMeas.boundingBox = data.bounds.size();
                measurements.emplace_back(std::move(newMeas));
            }
            contours.emplace_back(std::move(contour));
        };

        consumeHolesOfConnectedComponents(edgeTemp, contourContinuation);

        cv::Mat highlightedContours = color.clone();
        const std::size_t n = contours.size();
        for (std::size_t i = 0; i < n; ++i) {

            cv::Scalar color((std::rand() & 255), (std::rand() & 255),
                             (std::rand() & 255));
            cv::drawContours(highlightedContours, contours, i,
                             /*cv::Scalar(0, 0, 255)*/ color);
        }
        showImage("Selected contours", highlightedContours);
        cv::imwrite(fn + ".contours.png", highlightedContours);
        cv::waitKey();
    }

    void handleImage(std::string const &fn) {
        BlobParams p;

        std::cout << "Handling image " << fn << std::endl;
        cv::Mat color = cv::imread(fn, cv::IMREAD_COLOR);
        cv::Mat gray;
        if (color.channels() > 1) {
            cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = color.clone();
        }
        // showImage("Original", gray);

        auto thresholdInfo = ImageThresholdInfo(ImageRangeInfo(gray), p);
        cv::Mat thresholded;
        cv::threshold(gray, thresholded, p.absoluteMinThreshold, 255,
                      cv::THRESH_TOZERO);
        showImage("Cleaned", thresholded);
        cv::Mat edge;
        cv::Laplacian(thresholded, edge, CV_8U, 5);
        showImage("Edges", edge);
        processImageAndEdges(fn, color, gray, edge, p);
    }
} // namespace vbtracker
} // namespace osvr
int main(int argc, char *argv[]) {
    for (int arg = 1; arg < argc; ++arg) {
        osvr::vbtracker::handleImage(argv[arg]);
    }
    return 0;
}
