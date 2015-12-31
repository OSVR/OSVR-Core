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
        /// Truncates the passed-in list to the length needed to look for a
        /// pattern, so it does not grow too long and waste space and time,
        /// and perhaps produce spurious Ids.
        /// @param[out] lastBright set to True if we determine that the LED is
        /// currently "bright"
        /// @return -1 for unknown (not enough information) and
        /// less than -1 for definitely not an LED (light sources will be
        /// constant, mis-tracked LEDs may produce spurious changes in the
        /// pattern for example).
        virtual int getId(int currentId, BrightnessList &brightnesses,
                          bool &lastBright, bool blobsKeepId) const = 0;

      protected:
    };

} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_LedIdentifier_h_GUID_674F7CDB_87AD_41AA_2475_134F2B4A3FF9
