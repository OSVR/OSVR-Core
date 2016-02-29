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

#ifndef INCLUDED_CVTwoStepProgressBar_h_GUID_259E874E_85B4_4AAD_0FDE_BA90B5D9DCD4
#define INCLUDED_CVTwoStepProgressBar_h_GUID_259E874E_85B4_4AAD_0FDE_BA90B5D9DCD4

// Internal Includes
// - none

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <tuple>

namespace osvr {
namespace vbtracker {
    void drawTwoStepProgressBar(cv::Mat &image, cv::Point location,
                                cv::Size size, std::size_t complete,
                                std::size_t partial, std::size_t incomplete) {
        static const auto RED = cv::Vec3b{0, 0, 255};
        static const auto YELLOW = cv::Vec3b{0, 255, 255};
        static const auto GREEN = cv::Vec3b{0, 255, 0};

        auto totalUnits = complete + partial + incomplete;
        double totalWidth = size.width;

        /// Helper lambdas to reduce duplicate code. Taking colors as vec3b
        /// because Scalar has too many implicit conversions.

        /// Most basic, drawing a bar starting at the right place, right height,
        /// of designated width and color.
        auto drawBar = [&](int width, cv::Vec3b const &color) {
            cv::rectangle(image, location,
                          location + cv::Point(width, size.height),
                          cv::Scalar(color), CV_FILLED);
        };

        /// Draw a bar that's a fraction of the total length, based on the units
        /// passed in (computing the fraction internally), in a given color.
        auto drawFraction = [&](std::size_t portionOfTotal,
                                cv::Vec3b const &color) {
            auto width = double(portionOfTotal) / totalUnits * totalWidth;
            drawBar(static_cast<int>(width), color);
        };

        /// Drawing overlapping rectangles to avoid noisy lines at borders. So,
        /// the bottom one will always be the full size... This we call the
        /// "base bar"
        bool haveBaseBar = false;
        auto drawBaseBar = [&](cv::Vec3b const &color) {
            drawBar(size.width, color);
            haveBaseBar = true;
        };

        using std::make_tuple;
        /// A list of the layers, bottom to top, with their individual portions
        /// and their color, to iterate through.
        auto layers = {
            make_tuple(incomplete, incomplete + partial + complete, RED),
            make_tuple(partial, partial + complete, YELLOW),
            make_tuple(complete, complete, GREEN)};

        /// Iterate through the layers.
        for (auto &layer : layers) {
            std::size_t portionOfTotal;
            std::size_t cumulative;
            cv::Vec3b color;
            std::tie(portionOfTotal, cumulative, color) = layer;
            if (portionOfTotal == 0) {
                /// skip empty sections
                continue;
            }

            if (haveBaseBar) {
                // Can just draw the fraction if we already have a base
                // Drawing the cumulative fraction, though, since we overlap.
                drawFraction(cumulative, color);
            } else {
                // Otherwise we get to draw the base.
                drawBaseBar(color);
            }
        }
    }

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_CVTwoStepProgressBar_h_GUID_259E874E_85B4_4AAD_0FDE_BA90B5D9DCD4
