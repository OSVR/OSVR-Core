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
#include "HDKLedIdentifierFactory.h"
#include "HDKLedIdentifier.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    /// @brief Helper for factory function.
    static inline LedIdentifierPtr
    createHDKLedIdentifier(const PatternStringList &patterns) {
        LedIdentifierPtr ret{new OsvrHdkLedIdentifier(patterns)};
        return ret;
    }

    // clang-format off
    /// @brief Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
              "..*.....*...*..."    //  7
            , "...*......*...*."    //  8
            , ".......*...*...*"    //  9
            , "......*...*..*.."    // 10
            , ".......*....*..*"    // 11
            , "..*.....*..*...."    // 12
            , "....*......*..*."    // 13
            , "....*..*....*..."    // 14
            , "..*...*........*"    // 15
            , "........*..*..*."    // 16
            , "..*..*.*........"    // 17
            , "....*...*.*....."    // 18
            , "...*.*........*."    // 19
            , "...*.....*.*...."    // 20
            , "....*.*......*.."    // 21
            , "*.......*.*....."    // 22
            , ".*........*.*..."    // 23
            , ".*.........*.*.."    // 24
            , "....*.*..*......"    // 25
            , ".*.*.*.........."    // 26
            , ".........*.**..."    // 27
            , "**...........*.."    // 28
            , ".*...**........."    // 29
            , ".........*....**"    // 30
            , "..*.....**......"    // 31
            , "*......**......."    // 32
            , "...*.......**..."    // 33
            , "...**.....*....."    // 34
            , ".**....*........"    // 35
            , "....**...*......"    // 36
            , "*...........**.."    // 37
            , "......**.*......"    // 38
            , ".............***"    // 39
            , "..........*....."    // 40
    };

    /// @brief Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
              "***...*........*"    //  1
            , "...****..*......"    //  2
            , "*.*..........***"    //  3
            , "**...........***"    //  4
            , "*....*....*....."    //  5
            , "...*....*...*..."    //  6
    };
    // clang-format on

    LedIdentifierPtr createHDKLedIdentifier(uint8_t sensor) {
        LedIdentifierPtr ret;
        switch (sensor) {
        case 0:
            ret = createHDKLedIdentifier(OsvrHdkLedIdentifier_SENSOR0_PATTERNS);
            break;
        case 1:
            ret = createHDKLedIdentifier(OsvrHdkLedIdentifier_SENSOR1_PATTERNS);
        default:
            BOOST_ASSERT_MSG(sensor < 2, "Valid sensors are only 0 or 1!");
            break;
        }
        return ret;
    }

    // clang-format off
    /// @brief Patterns found in the "HDK_random_images" folder, which has only
    /// 8 images for each and they are a random subset of the actual images.
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS = {
              "..*....." // 7
            , "...*...." // 8
            , ".......*" // 9
            , "......*." // 10
            , ".......*" // 11
            , "..*....." // 12
            , "....*..." // 13
            , "....*..*" // 14
            , "********" // 15
            , "........" // 16
            , "********" // 17
            , "....*..." // 18
            , "...*.*.." // 19
            , "...*...." // 20
            , "....*.*." // 21
            , "...*.***" // 22
            , ".*......" // 23
            , "..*...*." // 24
            , "....*.*." // 25
            , ".*.*.*.." // 26
            , "........" // 27
            , "**......" // 28
            , ".*...**." // 29
            , "........" // 30
            , "..*....." // 31
            , "*......*" // 32
            , "...*...." // 33
            , "...**..." // 34
            , ".......*" // 35
            , "*..*..*." // 36
            , "*......." // 37
            , "......**" // 38
            , "........" // 39
            , "........" // 40
    };
    // clang-format on
    LedIdentifierPtr createRandomHDKLedIdentifier() {
        return createHDKLedIdentifier(
            OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS);
    }
} // End namespace vbtracker
} // End namespace osvr
