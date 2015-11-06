/** @file
    @brief Header defining an aligned memory allocator.

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_AlignedMemory_h_GUID_552DA92F_420A_4ADB_BCD2_493486A1C7A1
#define INCLUDED_AlignedMemory_h_GUID_552DA92F_420A_4ADB_BCD2_493486A1C7A1

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <stdlib.h>
#include <memory>

namespace osvr {
namespace util {
    enum {
        /// The default (and core-utilized) alignment of imaging buffers, etc.
        OSVR_DEFAULT_ALIGN_SIZE = 16
    };

    /// @brief Aligned allocation function, gives a pointer to a block of
    /// memory aligned to a memory boundary.
    inline void *aligned_alloc(size_t bytes,
                               size_t alignment = OSVR_DEFAULT_ALIGN_SIZE) {
        // Allocate a memory buffer with enough space to store a pointer to the
        // original buffer.
        const auto space = bytes + alignment + sizeof(void *);
        void *buffer = malloc(space);

        // Reserve the first byte for our buffer pointer for later freeing.
        void *aligned = (void **)buffer + 1;

        // After this call the 'aligned' pointer will be aligned to a boundary.
        // If there is not enough space to align the pointer, it stays
        // unaligned.
        std::align(alignment, bytes, aligned, space);

        // Store the buffer pointer for the delete call.
        ((void **)aligned)[-1] = buffer;

        return aligned;
    }

    /// @brief Aligned deallocation function, uses the pointer to the original
    /// memory block to deallocate it.
    inline void aligned_free(void *p) {
        // Null-pointer should be a no-op.
        if (p) {
            free(((void **)p)[-1]);
        }
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_AlignedMemory_h_GUID_552DA92F_420A_4ADB_BCD2_493486A1C7A1
