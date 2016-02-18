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

#ifndef INCLUDED_IdentifierHelpers_h_GUID_B6F81E02_BE7B_4382_12E5_87296135997D
#define INCLUDED_IdentifierHelpers_h_GUID_B6F81E02_BE7B_4382_12E5_87296135997D

// Internal Includes
#include "BasicTypes.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>

namespace osvr {
namespace vbtracker {

    /// @brief Helper for implementations of LedIdentifier to truncate the
    /// passed-in brightness list to the maximum useful length.
    inline void truncateBrightnessListTo(BrightnessList &brightnesses,
                                         size_t n) {
        /// @todo has to be a more efficient method.
        while (brightnesses.size() > n) {
            brightnesses.pop_front();
        }
    }

    /// @brief Helper function for implementations of LedIdentifier to find
    /// the minimum and maximum values in a non-empty list of brightnesses.
    inline BrightnessMinMax
    findMinMaxBrightness(const BrightnessList &brightnesses) {

        BOOST_ASSERT_MSG(!brightnesses.empty(), "Must be a non-empty list!");
        auto extremaIterators =
            std::minmax_element(begin(brightnesses), end(brightnesses));
        return std::make_pair(*extremaIterators.first,
                              *extremaIterators.second);
    }

    /// @brief Helper for implementations of LedIdentifier to turn a
    /// brightness list into a boolean list based on thresholding on the
    /// halfway point between minimum and maximum brightness.
    inline LedPatternWrapped
    getBitsUsingThreshold(const BrightnessList &brightnesses, float threshold) {
        LedPatternWrapped ret;
        // Allocate output space for our transform.
        ret.resize(brightnesses.size());

        // Transform the brightnesses into a string with '.' for dim
        // and '*' for bright.
        std::transform(begin(brightnesses), end(brightnesses), begin(ret),
                       [threshold](Brightness val) {
                           if (val >= threshold) {
                               return '*';
                           } else {
                               return '.';
                           }
                       });

        return ret;
    }
} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_IdentifierHelpers_h_GUID_B6F81E02_BE7B_4382_12E5_87296135997D
