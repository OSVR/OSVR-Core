/** @file
    @brief Header file for class that identifies LEDs based on blink codes.

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

#ifndef INCLUDED_LedIdentifier_h_GUID_674F7CDB_87AD_41AA_2475_134F2B4A3FF9
#define INCLUDED_LedIdentifier_h_GUID_674F7CDB_87AD_41AA_2475_134F2B4A3FF9

// Internal Includes
#include "Types.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    /// @brief Helper class to identify an LED based on its pattern of
    /// brightness over time.  The base class defines the interface.  The
    /// derived classes encode the pattern-detection algorithm for specific
    /// devices.
    ///
    /// NOTE: This class may modify the passed-in list, truncating it so that
    /// old data points are removed once there are enough measurements to make
    /// an estimate.
    ///
    /// @todo Consider adding a distance estimator as a parameter throughout,
    /// which can be left alone for unknown or estimated based on a Kalman
    /// filter; it would be used to scale the expected brightness.
    class LedIdentifier {
      public:
        /// @brief Virtual destructor;
        virtual ~LedIdentifier();
        /// @brief Determine the identity of the LED whose brightness pattern is
        /// passed in.
        /// @return -1 for unknown (not enough information) and
        /// less than -1 for definitely not an LED (light sources will be
        /// constant, mis-tracked LEDs may produce spurious changes in the
        /// pattern for example).
        virtual int getId(BrightnessList brightnesses) const = 0;

      protected:
        /// @brief Helper method for all derived classes to use to truncate the
        /// passed-in brightness list to the maximum useful length.
        /// @todo static method?
        void truncateBrightnessListTo(BrightnessList &brightnesses,
                                      size_t n) const;

        /// @brief Helper function to find the minimum and maximum values in a
        /// list of brightnesses.
        /// @return false if there is an empty list passed in.
        /// @todo static method?
        bool findMinMaxBrightness(const BrightnessList &brightnesses,
                                  Brightness &minVal, Brightness &maxVal) const;

        /// @brief Helper method for all derived classes to use to turn a
        /// brightness list into a boolean list based on thresholding on the
        /// halfway point between minimum and maximum brightness.
        /// @todo static method?
        LedPattern getBitsUsingThreshold(const std::list<float> &brightnesses,
                                         float threshold) const;
    };

    // Determines the LED IDs for the OSVR HDK
    extern const PatternStringList OsvrHdkLedIdentifier_SENSOR0_PATTERNS;
    extern const PatternStringList OsvrHdkLedIdentifier_SENSOR1_PATTERNS;
    extern const PatternStringList OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS;

    class OsvrHdkLedIdentifier : public LedIdentifier {
      public:
        /// @brief Give it a list of patterns to use.  There is a string for
        /// each LED, and each is encoded with '*' meaning that the LED is
        /// bright and '.' that it is dim at this point in time. All patterns
        /// must have the same length.
        OsvrHdkLedIdentifier(const PatternStringList &PATTERNS);

        ~OsvrHdkLedIdentifier() override;

        int getId(BrightnessList brightnesses) const override;

      private:
        size_t d_length;        //< Length of all patterns
        PatternList d_patterns; //< Patterns by index
    };

} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_LedIdentifier_h_GUID_674F7CDB_87AD_41AA_2475_134F2B4A3FF9
