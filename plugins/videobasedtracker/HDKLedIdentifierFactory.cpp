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
    /// These estimates are from the original documentation, not from the
    /// as-built measurements.
    /// First number in comments is overall LED ID, second is LED ID in the
    /// sensor it was in
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR0_PATTERNS_ORIGINAL = {
              "..*.....*...*..."    //  7  1
            , "...*......*...*."    //  8  2
            , ".......*...*...*"    //  9  3
            , "......*...*..*.."    // 10  4
            , ".......*....*..*"    // 11  5
            , "..*.....*..*...."    // 12  6
            , "....*......*..*."    // 13  7
            , "....*..*....*..."    // 14  8
            , "..*...*........*"    // 15  9
            , "........*..*..*."    // 16 10
            , "..*..*.*........"    // 17 11
            , "....*...*.*....."    // 18 12
            , "...*.*........*."    // 19 13
            , "...*.....*.*...."    // 20 14
            , "....*.*......*.."    // 21 15
            , "*.......*.*....."    // 22 16
            , ".*........*.*..."    // 23 17
            , ".*.........*.*.."    // 24 18
            , "....*.*..*......"    // 25 19
            , ".*.*.*.........."    // 26 20
            , ".........*.**..."    // 27 21
            , "**...........*.."    // 28 22
            , ".*...**........."    // 29 23
            , ".........*....**"    // 30 24
            , "..*.....**......"    // 31 25
            , "*......**......."    // 32 26 
            , "...*.......**..."    // 33 27
            , "...**.....*....."    // 34 28
            , ".**....*........"    // 35 29
            , "....**...*......"    // 36 30
            , "*...........**.."    // 37 31
            , "......**.*......"    // 38 32
            , ".............***"    // 39 33
            , "..........*....."    // 40 34
    };

    /// @brief Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
    /// These estimates are from the original documentation, not from the
    /// as-built measurements.
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR1_PATTERNS_ORIGINAL = {
              "***...*........*"    //  1  1
            , "...****..*......"    //  2  2
            , "*.*..........***"    //  3  3
            , "**...........***"    //  4  4
            , "*....*....*....."    //  5  5
            , "...*....*...*..."    //  6  6
    };

    /// @brief Determines the LED IDs for the OSVR HDK sensor 0 (face plate)
    /// These are from the as-built measurements.
    /// First number in comments is overall LED ID, second is LED ID in the
    /// sensor it was in
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR0_PATTERNS = {
            "X.**....*........"    //  5
          , "X....**...*......"    //  6
          , ".*...**........."    //  3
          , ".........*....**"    //  4
          , "..*.....**......"    //  1
          , "*......**......."    //  2 
          , "....*.*..*......"    // 10
          , ".*.*.*.........."    //  8
          , ".........*.**..."    //  9
          , "X**...........*.."    //  7
          , "....*.*......*.."    // 11
          , "X*.......*.*....."    // 28
          , "X.*........*.*..."    // 27
          , "X.*.........*.*.."    // 25
          , "..*..*.*........"    // 15
          , "....*...*.*....."    // 16
          , "...*.*........*."    // 17
          , "...*.....*.*...."    // 18
          , "....*......*..*."    // 19
          , "....*..*....*..."    // 20
          , "X..*...*........*"    // 21
          , "........*..*..*."    // 22
          , ".......*...*...*"    // 23
          , "......*...*..*.."    // 24
          , ".......*....*..*"    // 14
          , "..*.....*..*...."    // 26
          , "*....*....*....."    // 13
          , "...*....*...*..."    // 12
          , "..*.....*...*..."    // 29
          , "...*......*...*."    // 30
          , "***...*........*"    // 31
          , "...****..*......"    // 32
          , "*.*..........***"    // 33
          , "**...........***"    // 34
    };

    /// @brief Determines the LED IDs for the OSVR HDK sensor 1 (back plate)
    /// These are from the as-built measurements.
    static const std::vector<std::string>
        OsvrHdkLedIdentifier_SENSOR1_PATTERNS = {
            "X............**.."    // 37 31 // never actually turns on in production
          , "......**.*......"    // 38 32
          , ".............***"    // 39 33
          , "X..........*....."    // 40 34 // never actually turns on in production
          , "...*.......**..."    // 33 27
          , "...**.....*....."    // 34 28
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

    LedIdentifierPtr createHDKUnifiedLedIdentifier() {
        LedIdentifierPtr ret;
        std::vector<std::string> patterns = OsvrHdkLedIdentifier_SENSOR0_PATTERNS;
        patterns.insert(end(patterns),
                        begin(OsvrHdkLedIdentifier_SENSOR1_PATTERNS),
                        end(OsvrHdkLedIdentifier_SENSOR1_PATTERNS));
        ret = createHDKLedIdentifier(patterns);
        return ret;
    }

    LedIdentifierPtr createHDKLedIdentifierSimulated(uint8_t sensor) {
      LedIdentifierPtr ret;
      switch (sensor) {
      case 0:
        ret = createHDKLedIdentifier(OsvrHdkLedIdentifier_SENSOR0_PATTERNS_ORIGINAL);
        break;
      case 1:
        ret = createHDKLedIdentifier(OsvrHdkLedIdentifier_SENSOR1_PATTERNS_ORIGINAL);
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
