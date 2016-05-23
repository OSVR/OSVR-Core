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

#ifndef INCLUDED_cvUtils_h_GUID_65B004C2_722B_4BBF_4EC7_05B2AD861254
#define INCLUDED_cvUtils_h_GUID_65B004C2_722B_4BBF_4EC7_05B2AD861254

// Internal Includes
#include <BlobExtractor.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

namespace osvr {
namespace vbtracker {
    inline cv::Point2f castPointToFloat(cv::Point2d const &p) {
        return cv::Point2f(static_cast<float>(p.x), static_cast<float>(p.y));
    }

    inline void drawSubpixelPoint(cv::Mat image, cv::Point2d point,
                                  cv::Scalar color = cv::Scalar(0, 0, 0),
                                  double radius = 1.,
                                  std::uint8_t fractionalBits = 3) {
        const auto SHIFT = std::pow(2, fractionalBits);
        auto intPoint = cv::Point(static_cast<int>(point.x * SHIFT),
                                  static_cast<int>(point.y * SHIFT));
        cv::circle(image, intPoint, static_cast<int>(radius * SHIFT), color, -1,
                   8, fractionalBits);
    }

    /// Draw contours on a copy of the base image (converted to BGR, if
    /// required), with each contour a potentially a unique color thanks to
    /// functors! If you choose not to fill the contours, the center of each
    /// will have a subpixel-accurate circle plotted at it.
    ///
    /// @param baseImage Image to have the contours drawn on top of - will not
    /// be modified - just cloned.
    /// @param contours The contours you'd like drawn.
    /// @param colorFunc a callable taking ContourType const &, std::size_t
    /// (index) and returning a cv::Scalar color.
    /// @param fillContours Whether contours should be drawn filled, or just as
    /// outlines with a center point plotted (default)
    /// @param centerDotRadius Radius, in pixels, of the center dot to plot if
    /// fillContours is false.
    /// @param colorCenterDot Whether the center dots drawn if fillContours is
    /// false should match the corresponding contour color (default), or if they
    /// should all be black.
    template <typename F>
    inline cv::Mat drawFunctorColoredContours(
        cv::Mat const &baseImage, std::vector<ContourType> const &contours,
        F &&colorFunc, bool fillContours = false, double centerDotRadius = 1.2,
        bool colorCenterDot = true) {
        cv::Mat highlightedContours;
        if (baseImage.channels() > 1) {
            highlightedContours = baseImage.clone();
        } else {
            cv::cvtColor(baseImage, highlightedContours, cv::COLOR_GRAY2BGR);
        }
        const cv::Scalar black(0, 0, 0);
        const std::size_t n = contours.size();
        for (std::size_t i = 0; i < n; ++i) {
            cv::Scalar color = std::forward<F>(colorFunc)(contours[i], i);
            cv::drawContours(highlightedContours, contours, i, color,
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

    /// Draw contours on a copy of the base image (converted to BGR, if
    /// required), with each contour a unique color thanks to a Mersenne
    /// Twister! If you choose not to fill the contours, the center of each
    /// will have a subpixel-accurate circle plotted at it.
    ///
    /// @param baseImage Image to have the contours drawn on top of - will not
    /// be modified - just cloned.
    /// @param contours The contours you'd like drawn.
    /// @param deterministic Should we use a deterministic seed?
    /// @param fillContours Whether contours should be drawn filled, or just as
    /// outlines with a center point plotted (default)
    /// @param centerDotRadius Radius, in pixels, of the center dot to plot if
    /// fillContours is false.
    /// @param colorCenterDot Whether the center dots drawn if fillContours is
    /// false should match the corresponding contour color (default), or if they
    /// should all be black.
    inline cv::Mat drawColoredContours(cv::Mat const &baseImage,
                                       std::vector<ContourType> const &contours,
                                       bool deterministic = true,
                                       bool fillContours = false,
                                       double centerDotRadius = 1.2,
                                       bool colorCenterDot = true) {
        // Using a deterministic 32-bit seed here, so we get the same colors in
        // each call - by default.
        std::mt19937 mt(31415);
        if (!deterministic) {
            // If you didn't want deterministic, ok, we'll go get the random
            // device and seed it.
            std::random_device rd;
            mt.seed(rd());
        }

        // std::uniform_int_distribution<int> colorDist(0, 255);
        /// Our desired range is a power of two, so we can just mask.
        auto colorDist = [](std::mt19937 &mt) { return mt() & 0xff; };
        return drawFunctorColoredContours(
            baseImage, contours,
            [&](ContourType const &, std::size_t) {
                return cv::Scalar(colorDist(mt), colorDist(mt), colorDist(mt));
            },
            fillContours, centerDotRadius, colorCenterDot);
    }

    /// Draw contours on a copy of the base image (converted to BGR, if
    /// required), with all contours the same color. If you choose not to fill
    /// the contours, the center of each will have a subpixel-accurate circle
    /// plotted at it.
    ///
    /// @param baseImage Image to have the contours drawn on top of - will not
    /// be modified - just cloned.
    /// @param contours The contours you'd like drawn.
    /// @param color The color you want the contour drawn.
    /// @param fillContours Whether contours should be drawn filled, or just as
    /// outlines with a center point plotted (default)
    /// @param centerDotRadius Radius, in pixels, of the center dot to plot if
    /// fillContours is false.
    /// @param colorCenterDot Whether the center dots drawn if fillContours is
    /// false should match the corresponding contour color (default), or if they
    /// should all be black.
    inline cv::Mat drawSingleColoredContours(
        cv::Mat const &baseImage, std::vector<ContourType> const &contours,
        cv::Scalar color, bool fillContours = false,
        double centerDotRadius = 1.2, bool colorCenterDot = true) {
        return drawFunctorColoredContours(
            baseImage, contours,
            [&color](ContourType const &, std::size_t) { return color; },
            fillContours, centerDotRadius, colorCenterDot);
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
        // intentionally storing in int, instead of auto, since we'll compare
        // against int.
        int n = contours.size();
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
    void consumeHolesOfConnectedComponents(cv::InputOutputArray input,
                                           F &&continuation) {
        std::vector<ContourType> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(input, contours, hierarchy, cv::RETR_CCOMP,
                         cv::CHAIN_APPROX_NONE);
        // intentionally storing in int, instead of auto, since we'll compare
        // against int.
        int n = contours.size();
        /// Loop through the outside connected components.
        for (int outsides = 0; outsides >= 0 && outsides < n;
             outsides = hierarchy[outsides][HIERARCHY_NEXT_SIBLING_CONTOUR]) {
            for (int idx = hierarchy[outsides][HIERARCHY_FIRST_CHILD_CONTOUR];
                 idx >= 0 && idx < n;
                 idx = hierarchy[idx][HIERARCHY_NEXT_SIBLING_CONTOUR])
                /// We want all first-level children of connected components.
                std::forward<F>(continuation)(std::move(contours[idx]));
        }
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_cvUtils_h_GUID_65B004C2_722B_4BBF_4EC7_05B2AD861254
