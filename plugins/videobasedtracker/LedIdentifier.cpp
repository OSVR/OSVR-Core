/** @file
@brief Implementation for class that identifies LEDs based on blink codes.

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "LedIdentifier.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>

namespace osvr {
namespace vbtracker {
    LedIdentifier::~LedIdentifier() {}
    void LedIdentifier::truncateBrightnessListTo(BrightnessList &brightnesses,
                                                 size_t n) const {
        while (brightnesses.size() > n) {
            brightnesses.pop_front();
        }
    }

    BrightnessMinMax LedIdentifier::findMinMaxBrightness(
        const BrightnessList &brightnesses) const {
        BOOST_ASSERT_MSG(!brightnesses.empty(), "Must be a non-empty list!");
        auto extremaIterators =
            std::minmax_element(begin(brightnesses), end(brightnesses));
        return std::make_pair(*extremaIterators.first,
                              *extremaIterators.second);
    }

    LedPattern
    LedIdentifier::getBitsUsingThreshold(const BrightnessList &brightnesses,
                                         Brightness threshold) const {
        LedPattern ret;
        // Allocate output space for our transform.
        ret.resize(brightnesses.size());

        std::transform(
            begin(brightnesses), end(brightnesses), begin(ret),
            [threshold](Brightness val) { return val >= threshold; });

        return ret;
    }

    // Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
    const std::vector<std::string> OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
        "..*.....*...*...", "...*......*...*.", ".......*...*...*",
        "......*...*..*..", ".......*....*..*", "..*.....*..*....",
        "....*......*..*.", "....*..*....*...", "..*...*........*",
        "........*..*..*.", "..*..*.*........", "....*...*.*.....",
        "...*.*........*.", "...*.....*.*....", "....*.*......*..",
        "*.......*.*.....", ".*........*.*...", ".*.........*.*..",
        "....*.*..*......", ".*.*.*..........", ".........*.**...",
        "**...........*..", ".*...**.........", ".........*....**",
        "..*.....**......", "*......**.......", "...*.......**...",
        "...**.....*.....", ".**....*........", "....**...*......",
        "*...........**..", "......**.*......", ".............***",
        "..........*....."};

    // Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
    const std::vector<std::string> OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
        "***...*........*", "...****..*......", "*.*..........***",
        "**...........***", "*....*....*.....", "...*....*...*..."};

    // Patterns found in the "HDK_random_images" folder, which has only 8
    // images for each and they are a random subset of the actual images.
    const std::vector<std::string> OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS =
        {
            "..*....." // 7
            ,
            "...*...." // 8
            ,
            ".......*" // 9
            ,
            "......*." // 10
            ,
            ".......*" // 11
            ,
            "..*....." // 12
            ,
            "....*..." // 13
            ,
            "....*..*" // 14
            ,
            "********" // 15
            ,
            "........" // 16
            ,
            "********" // 17
            ,
            "....*..." // 18
            ,
            "...*.*.." // 19
            ,
            "...*...." // 20
            ,
            "....*.*." // 21
            ,
            "...*.***" // 22
            ,
            ".*......" // 23
            ,
            "..*...*." // 24
            ,
            "....*.*." // 25
            ,
            ".*.*.*.." // 26
            ,
            "........" // 27
            ,
            "**......" // 28
            ,
            ".*...**." // 29
            ,
            "........" // 30
            ,
            "..*....." // 31
            ,
            "*......*" // 32
            ,
            "...*...." // 33
            ,
            "...**..." // 34
            ,
            ".......*" // 35
            ,
            "*..*..*." // 36
            ,
            "*......." // 37
            ,
            "......**" // 38
            ,
            "........" // 39
            ,
            "........" // 40
    };
    OsvrHdkLedIdentifier::~OsvrHdkLedIdentifier() {}
    // Convert from string encoding representations into lists
    // of boolean values for use in comparison.
    OsvrHdkLedIdentifier::OsvrHdkLedIdentifier(
        const PatternStringList &PATTERNS) {
        // Ensure that we have at least one entry in our list and
        // find the length of the first entry.
        if (PATTERNS.size() == 0) {
            d_length = 0;
            return;
        }
        d_length = PATTERNS[0].size();

        // Decode each string into a new list of boolean values, making
        // sure each have the correct length.
        for (size_t i = 0; i < PATTERNS.size(); i++) {

            // Make sure the pattern is the correct length.
            if (PATTERNS[i].size() != d_length) {
                d_patterns.clear();
                d_length = 0;
                return;
            }

            // Make a new boolean-list encoding from it, replacing every
            // non-'.' character with true and every '.' with false.
            std::list<bool> pattern;
            for (size_t j = 0; j < PATTERNS[i].size(); j++) {
                if (PATTERNS[i][j] == '.') {
                    pattern.push_back(false);
                } else {
                    pattern.push_back(true);
                }
            }

            // Add the pattern to the vector of lists.
            d_patterns.push_back(pattern);
        }
        // std::cout << "XXX d_length = " << d_length << ", num patterns = " <<
        // d_patterns.size() << std::endl;
    }

    int OsvrHdkLedIdentifier::getId(std::list<float> brightnesses) const {
        // If we don't have at least the required number of frames of data, we
        // don't know anything.
        if (brightnesses.size() < d_length) {
            return -1;
        }

        // We only care about the d_length most-recent levels.
        truncateBrightnessListTo(brightnesses, d_length);

        // Compute the minimum and maximum brightness values.  If
        // they are too close to each other, we have a light rather
        // than an LED.  If not, compute a threshold to separate the
        // 0's and 1's.
        auto extrema = findMinMaxBrightness(brightnesses);
        Brightness minVal = extrema.first;
        Brightness maxVal = extrema.second;
        static const double TODO_MIN_BRIGHTNESS_DIFF = 0.5;
        if (maxVal - minVal <= TODO_MIN_BRIGHTNESS_DIFF) {
            return -2;
        }
        float threshold = (minVal + maxVal) / 2;

        // Get a list of boolean values for 0's and 1's using
        // the threshold computed above.
        std::list<bool> bits = getBitsUsingThreshold(brightnesses, threshold);

        // Search through the available patterns to see if the passed-in
        // pattern matches any of them.  If so, return that pattern.  We
        // need to check all potential rotations of the pattern, since we
        // don't know when the code started.  For the HDK, the codes are
        // rotationally invariant.
        for (size_t i = 0; i < d_patterns.size(); i++) {
            for (size_t j = 0; j < bits.size(); j++) {
                if (bits == d_patterns[i]) {
                    return static_cast<int>(i);
                }

                // So long as we don't find the solution, this rotates
                // back to the initial configuration after each inner loop.
                std::list<bool>::iterator mid = bits.begin();
                std::rotate(bits.begin(), ++mid, bits.end());
            }
        }

        // No pattern recognized and we should have recognized one, so return
        // a low negative.  We've used -2 so return -3.
        return -3;
    }

} // End namespace vbtracker
} // End namespace osvr
