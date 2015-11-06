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
#include <osvr/Util/AlignedMemoryC.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdlib.h>
#include <stdexcept>

namespace osvr {
namespace util {

    /// @brief Aligned allocation function, gives a pointer to a block of
    /// memory aligned to a memory boundary.
    inline void *alignedAlloc(size_t bytes,
                              size_t alignment = OSVR_DEFAULT_ALIGN_SIZE) {
        void *ret = osvrAlignedAlloc(bytes, alignment);
        if (!ret) {
            throw std::runtime_error("Could not perform aligned allocation!");
        }
        return ret;
    }

    /// @brief Aligned deallocation function, uses the pointer to the original
    /// memory block to deallocate it.
    inline void alignedFree(void *p) { osvrAlignedFree(p); }

    /// @brief Deleter class matching alignedAlloc
    class AlignedAllocDeleter {
      public:
        void operator()(void *p) const { alignedFree(p); }
    };
} // namespace util
} // namespace osvr
#endif // INCLUDED_AlignedMemory_h_GUID_552DA92F_420A_4ADB_BCD2_493486A1C7A1
