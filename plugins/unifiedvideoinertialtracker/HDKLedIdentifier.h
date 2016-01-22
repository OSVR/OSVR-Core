/** @file
    @brief Header

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

#ifndef INCLUDED_HDKLedIdentifier_h_GUID_A1204292_1F90_459E_44AD_18BA932C0E93
#define INCLUDED_HDKLedIdentifier_h_GUID_A1204292_1F90_459E_44AD_18BA932C0E93

// Internal Includes
#include "LedIdentifier.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    class OsvrHdkLedIdentifier : public LedIdentifier {
      public:
        /// @brief Give it a list of patterns to use.  There is a string for
        /// each LED, and each is encoded with '*' meaning that the LED is
        /// bright and '.' that it is dim at this point in time. All patterns
        /// must have the same length.
        OsvrHdkLedIdentifier(const PatternStringList &PATTERNS);

        ~OsvrHdkLedIdentifier() override;

        /// @brief Determine an ID based on a list of brightnesses
        /// This truncates the passed-in list to only as many elements
        /// as are in the pattern list, to keep it from growing too
        /// large and wasting time and space.
        ZeroBasedBeaconId getId(ZeroBasedBeaconId currentId,
                                BrightnessList &brightnesses, bool &lastBright,
                                bool blobsKeepId) const override;

      private:
        size_t d_length;        //< Length of all patterns
        PatternList d_patterns; //< Patterns by index
    };

} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_HDKLedIdentifier_h_GUID_A1204292_1F90_459E_44AD_18BA932C0E93
