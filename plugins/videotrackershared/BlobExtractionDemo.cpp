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
#include <EdgeHoleBasedLedExtractor.h>
#include <OptionalStream.h>
#include <cvUtils.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <cmath>
#include <iostream>

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
        EdgeHoleBasedLedExtractor extractor;
        extractor(gray, p, true);

        /// Basic thresholding to reduce background noise
        showImage("Cleaned", extractor.getThresholdedImage());
        cv::imwrite(fn + ".thresh.png", extractor.getThresholdedImage());

        /// Edge detection
        showImage("Edges", extractor.getEdgeDetectedImage());
        cv::imwrite(fn + ".edge.png", extractor.getEdgeDetectedImage());

        /// Extract beacons from the edge detection image

        /// Produce a colored debug image where each accepted contour is
        /// encircled with a different color.
        cv::Mat highlightedContours =
            drawColoredContours(gray, extractor.getContours());
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
