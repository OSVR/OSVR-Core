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

#include "LedIdentifier.h"

namespace osvr {
namespace vbtracker {

void LedIdentifier::truncateBrightnessListTo(std::list<float> &brightnesses, size_t n) const {
    while (brightnesses.size() > n) {
        brightnesses.pop_front();
    }
}

// Helper function to find the minimum and maximum values in a
// list of brightnesses.  Returns false if there is an empty
// list passed in.
bool LedIdentifier::findMinMaxBrightness(const std::list<float> &brightnesses
    , float &minVal, float &maxVal) const
{
    if (brightnesses.size() == 0) {
        minVal = maxVal = 0;
        return false;
    }

    // Find the largest and smallest values, and then
    // the threshold.
    std::list<float>::const_iterator i = brightnesses.begin();
    minVal = *i;
    maxVal = minVal;
    while (++i != brightnesses.end()) {
        if (*i < minVal) { minVal = *i; }
        if (*i > maxVal) { maxVal = *i; }
    }
    return true;
}

// Helper method for all derived classes to use to turn a brightness
// list into a boolean list based on thresholding on the halfway
// point between minumum and maximum brightness.
std::list<bool> LedIdentifier::getBitsUsingThreshold(
    const std::list<float> &brightnesses
    , float threshold) const
{
    std::list<bool> ret;

    // Categorize each element according to above or below threshold.
    std::list<float>::const_iterator i;
    for (i = brightnesses.begin(); i != brightnesses.end(); i++) {
        if (*i >= threshold) {
            ret.push_back(true);
        }
        else {
            ret.push_back(false);
        }
    }
    return ret;
}

// Determines the LED IDs for the OSVR HDK
const std::vector<std::string> OsvrHdkLedIdentifier_DEFAULT_PATTERNS = {
      "***...*........*"
    , "...****..*......"
    , "*.*..........***"
    , "**...........***"
    , "*....*....*....."
    , "...*....*...*..."
    , "..*.....*...*..."
    , "...*......*...*."
    , ".......*...*...*"
    , "......*...*..*.."
    , ".......*....*..*"
    , "..*.....*..*...."
    , "....*......*..*."
    , "....*..*....*..."
    , "..*...*........*"
    , "........*..*..*."
    , "..*..*.*........"
    , "....*...*.*....."
    , "...*.*........*."
    , "...*.....*.*...."
    , "....*.*......*.."
    , "*.......*.*....."
    , ".*........*.*..."
    , ".*.........*.*.."
    , "....*.*..*......"
    , ".*.*.*.........."
    , ".........*.**..."
    , "**...........*.."
    , ".*...**........."
    , ".........*....**"
    , "..*.....**......"
    , "*......**......."
    , "...*.......**..."
    , "...**.....*....."
    , ".**....*........"
    , "....**...*......"
    , "*...........**.."
    , "......**.*......"
    , ".............***"
    , "..........*....."
};

// Patterns found in the "HDK_random_images" folder, which has only 8
// images for each and they are a random subset of the actual images.
const std::vector<std::string> OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS = {
      "***...*."
    , "...****."
    , "*.*....."
    , "**......"
    , "*....*.."
    , "...*...."
    , "..*....."
    , "...*...."
    , ".......*"
    , "......*."
    , ".......*"
    , "..*....."
    , "....*..."
    , "....*..*"
    , "..*...*."
    , "........"
    , "..*..*.*"
    , "....*..."
    , "...*.*.."
    , "...*...."
    , "....*.*."
    , "*......."
    , ".*......"
    , ".*......"
    , "....*.*."
    , ".*.*.*.."
    , "........"
    , "**......"
    , ".*...**."
    , "........"
    , "..*....."
    , "*......*"
    , "...*...."
    , "...**..."
    , ".**....*"
    , "....**.."
    , "*......."
    , "......**"
    , "........"
    , "........"
};

// Convert from string encoding representations into lists
// of boolean values for use in comparison.
OsvrHdkLedIdentifier::OsvrHdkLedIdentifier(const std::vector<std::string> &PATTERNS)
{
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
    //std::cout << "XXX d_length = " << d_length << ", num patterns = " << d_patterns.size() << std::endl;
}

int OsvrHdkLedIdentifier::getId(std::list<float> brightnesses) const
{
    // If we don't have at least the required number of frames of data, we don't
    // know anything.
    if (brightnesses.size() < d_length) { return -1; }

    // We only care about the d_length most-recent levels.
    truncateBrightnessListTo(brightnesses, d_length);

    // Compute the minimum and maximum brightness values.  If
    // they are too close to each other, we have a light rather
    // than an LED.  If not, compute a threshold to separate the
    // 0's and 1's.
    float minVal, maxVal;
    if (!findMinMaxBrightness(brightnesses, minVal, maxVal)) {
        return -2;
    }
    static const double TODO_MIN_BRIGHTNESS_DIFF = 0.5;
    if (maxVal - minVal <= TODO_MIN_BRIGHTNESS_DIFF) { return -2; }
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
