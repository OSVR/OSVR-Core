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
    int edgeThresh = 8;

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
            if (hierarchy[i][3] < 0 &&
                std::forward<F>(additionalPredicate)(contours[i])) {
                ret.emplace_back(std::move(contours[i]));
            }
        }
        return ret;
    }

    void processImageAndEdges(cv::Mat color, cv::Mat gray, cv::Mat edge,
                              BlobParams const &p) {
        std::vector<ContourType> contours;
        {
            cv::Mat edgeTemp = edge.clone();
            auto contourPredicate = [&p](ContourType const &contour) {
                auto data = getBlobDataFromContour(contour);
                if (p.filterByCircularity) {
                    if (data.circularity < p.minCircularity) {
                        return false;
                    }
                }
                if (data.area < p.minArea) {
                    return false;
                }
                return true;
            };
            contours =
                getOutsidesOfConnectedComponents(edgeTemp, contourPredicate);
        }
        cv::Mat highlightedContours = color.clone();
        cv::drawContours(highlightedContours, contours, -1,
                         cv::Scalar(0, 0, 255));
        showImage("Selected contours", highlightedContours);
        cv::waitKey();
    }

    void handleImage(std::string const &fn) {
        BlobParams p;

        std::cout << "Handling image " << fn << std::endl;
        cv::Mat color = cv::imread(fn, cv::IMREAD_COLOR);
        cv::Mat gray;
        cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);
        // showImage("Original", gray);

        auto thresholdInfo = ImageThresholdInfo(ImageRangeInfo(gray), p);
        cv::Mat thresholded;
        cv::threshold(gray, thresholded, p.absoluteMinThreshold, 255,
                      cv::THRESH_TOZERO);
        showImage("Cleaned", thresholded);

        cv::Mat edge;
        cv::Canny(thresholded, edge, edgeThresh, edgeThresh * 3, 3);
        showImage("Edges", edge);
        processImageAndEdges(color, gray, edge, p);
    }
} // namespace vbtracker
} // namespace osvr
int main(int argc, char *argv[]) {
    for (int arg = 1; arg < argc; ++arg) {
        osvr::vbtracker::handleImage(argv[arg]);
    }
    return 0;
}
