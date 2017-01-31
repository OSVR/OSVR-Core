/** @file
    @brief Header

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

#ifndef INCLUDED_GenerateBlobDebugImage_h_GUID_558B5666_8637_48B9_A215_E7A1B798DDD8
#define INCLUDED_GenerateBlobDebugImage_h_GUID_558B5666_8637_48B9_A215_E7A1B798DDD8

// Internal Includes
#include <EdgeHoleBasedLedExtractor.h>
#include <cvUtils.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <random>
#include <sstream>
#include <string>

namespace osvr {
namespace vbtracker {
    inline cv::Mat
    generateBlobDebugImage(cv::Mat const &inputImage,
                           EdgeHoleBasedLedExtractor const &extractor) {

        /// Produce a colored debug image where each accepted contour is
        /// encircled with a different color.
        cv::Mat highlightedContours =
            drawColoredContours(inputImage, extractor.getContours());

        // Figure out where to put the text - further along the ray
        // that passes from the image center through the reject center, to try
        // to get it "away from the crowd"
        {
            static const auto FONT_FACE = cv::FONT_HERSHEY_PLAIN;
            static const auto FONT_SCALE = 0.8;
            static const int FONT_THICKNESS = 1;

            auto imageSize = highlightedContours.size();
            auto imageCenter =
                cv::Point2d(imageSize.width / 2, imageSize.height / 2);
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<double> vectorScaleDistribution(1.1,
                                                                           1.3);
            auto computeTextLocation = [&](cv::Point2d rejectCenter,
                                           std::string const &text) {
                auto centerToRejectVec = rejectCenter - imageCenter;
                auto textPoint =
                    cv::Point(centerToRejectVec * vectorScaleDistribution(mt) +
                              imageCenter);

                /// OK, so that's where we'll center the text, now we have to
                /// see how big the text is to offset it...
                auto textSize = cv::getTextSize(text, FONT_FACE, FONT_SCALE,
                                                FONT_THICKNESS, nullptr);

                auto textOffset =
                    cv::Point(textSize.width / 2, textSize.height / 2);
                return textPoint - textOffset;
            };

            /// Draw and label the rejected centers of contours.
            for (auto &reject : extractor.getRejectList()) {
                EdgeHoleBasedLedExtractor::ContourId contourId = 0;
                RejectReason reason;
                cv::Point2d center;
                std::tie(contourId, reason, center) = reject;

                if (osvr::vbtracker::RejectReason::Area == reason ||
                    RejectReason::CenterPointValue == reason) {
                    // Skip drawing these, they clutter the display
                    continue;
                }
                drawSubpixelPoint(highlightedContours, center,
                                  cv::Scalar(0, 0, 0), 1.2);
                std::ostringstream os;
                // os << "[";
                os << contourId;
                switch (reason) {
                case RejectReason::Area:
                    os << ":AREA";
                    break;
                case RejectReason::CenterPointValue:
                    os << ":VAL";
                    break;
                case RejectReason::Circularity:
                    os << ":CIRC";
                    break;
                case RejectReason::Convexity:
                    os << ":CONV";
                    break;
                default:
                    break;
                }
                // os << "]";
                auto text = os.str();
                cv::putText(highlightedContours, text,
                            computeTextLocation(center, text), FONT_FACE,
                            FONT_SCALE, cv::Scalar(0, 0, 255), FONT_THICKNESS);
            }
        }

        return highlightedContours;
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_GenerateBlobDebugImage_h_GUID_558B5666_8637_48B9_A215_E7A1B798DDD8
