/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "LED.h"
#include "HDKLedIdentifier.h"
#include "IdentifierHelpers.h"

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace osvr {
namespace vbtracker {
    static const auto VALIDCHARS = "*.";
    OsvrHdkLedIdentifier::~OsvrHdkLedIdentifier() {}
    // Convert from string encoding representations into lists
    // of boolean values for use in comparison.
    OsvrHdkLedIdentifier::OsvrHdkLedIdentifier(
        const PatternStringList &PATTERNS) {
        // Ensure that we have at least one entry in our list and
        // find the length of the first valid entry.
        d_length = 0;
        if (PATTERNS.empty()) {
            return;
        }

        for (auto &pat : PATTERNS) {
            if (!pat.empty() && pat.find_first_not_of(VALIDCHARS) == pat.npos) {
                // All characters in this pattern are valid - we can consider
                // this to be a valid pattern and thus establish the size.
                d_length = pat.length();
                break;
            }
        }

        if (0 == d_length) {
            // If we still have 0 as the pattern length, return.
            return;
        }

        // Decode each string into a new list of boolean values, making
        // sure each have the correct length.

        for (auto &pat : PATTERNS) {
            if (pat.empty() || pat.find_first_not_of(VALIDCHARS) != pat.npos) {
                // This is an intentionally disabled beacon/pattern.
                d_patterns.emplace_back();
                continue;
            }

            // Make sure the pattern is the correct length.
            if (pat.size() != d_length) {
                throw std::runtime_error("Got a pattern of incorrect length!");
            }

            // Make a wrapped pattern, which is the original pattern plus
            // a second copy of the pattern that has all but the last
            // character in it.  This will enable use to use the string
            // find() routine to see if any shift of the pattern is a
            // match.
            auto wrapped = pat + pat;
            wrapped.pop_back();
            d_patterns.push_back(std::move(wrapped));
        }
    }

    ZeroBasedBeaconId
    OsvrHdkLedIdentifier::getId(ZeroBasedBeaconId currentId,
                                std::list<float> &brightnesses,
                                bool &lastBright, bool blobsKeepId) const {
        // If we don't have at least the required number of frames of data, we
        // don't know anything.
        if (brightnesses.size() < d_length) {
            return ZeroBasedBeaconId(
                Led::SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA);
        }

        // We only care about the d_length most-recent levels.
        truncateBrightnessListTo(brightnesses, d_length);

        // Compute the minimum and maximum brightness values.  If
        // they are too close to each other, we have a light rather
        // than an LED.  If not, compute a threshold to separate the
        // 0's and 1's.
        Brightness minVal, maxVal;
        std::tie(minVal, maxVal) = findMinMaxBrightness(brightnesses);
        // Brightness is currently actually keypoint diameter (radius?) in
        // pixels, and it's being under-estimated by OpenCV.
        static const double TODO_MIN_BRIGHTNESS_DIFF = 0.3;
        if (maxVal - minVal <= TODO_MIN_BRIGHTNESS_DIFF) {
            return ZeroBasedBeaconId(
                Led::SENTINEL_INSUFFICIENT_EXTREMA_DIFFERENCE);
        }
        const auto threshold = (minVal + maxVal) / 2;
        // Set the `lastBright` out variable
        lastBright = brightnesses.back() >= threshold;

        if (blobsKeepId && beaconIdentified(currentId)) {
            // Early out if we already have identified this LED.
            return currentId;
        }

        // Get a list of boolean values for 0's and 1's using
        // the threshold computed above.
        auto bits = getBitsUsingThreshold(brightnesses, threshold);

        // Search through the available patterns to see if the passed-in
        // pattern matches any of them.  If so, return that pattern.  We
        // need to check all potential rotations of the pattern, since we
        // don't know when the code started.  For the HDK, the codes are
        // rotationally invariant.  We do this by making wrapped strings
        // and seeing if the pattern shows up anywhe in them, relying on
        // the std::string find method to do efficiently.
        for (size_t i = 0; i < d_patterns.size(); i++) {
            if (d_patterns[i].empty()) {
                /// Skip turned-off patterns.
                continue;
            }
            if (d_patterns[i].find(bits) != std::string::npos) {
                return ZeroBasedBeaconId(i);
            }
        }

        // No pattern recognized and we should have recognized one, so return
        // a low negative.  We've used -2 so return -3.
        return ZeroBasedBeaconId(
            Led::SENTINEL_NO_PATTERN_RECOGNIZED_DESPITE_SUFFICIENT_DATA);
    }

} // End namespace vbtracker
} // End namespace osvr
