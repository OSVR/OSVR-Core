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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_AlignmentPadding_h_GUID_B916398A_5714_44B8_455A_B38EA1092172
#define INCLUDED_AlignmentPadding_h_GUID_B916398A_5714_44B8_455A_B38EA1092172

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <stddef.h>

namespace osvr {
namespace common {
    /// @brief Given an alignment in bytes, and a current size of a buffer,
    /// return the number of bytes of padding required to align the next field
    /// to be added to the buffer at the desired alignment within that buffer.
    ///
    /// That is, return some padding such that (currentSize + padding) %
    /// alignment == 0 for alignment > 1.
    ///
    /// @param alignment Alignment in bytes: both 0 and 1 are accepted to mean
    /// "no alignment"
    /// @param currentSize Current number of bytes in a buffer
    inline size_t computeAlignmentPadding(size_t alignment,
                                          size_t currentSize) {
        size_t ret = 0;
        if (2 > alignment) {
            /// No alignment requested
            return ret;
        }
        auto leftover = currentSize % alignment;
        if (leftover == 0) {
            /// Buffer is already aligned
            return ret;
        }
        /// Buffer needs some padding
        ret = alignment - leftover;
        return ret;
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_AlignmentPadding_h_GUID_B916398A_5714_44B8_455A_B38EA1092172
