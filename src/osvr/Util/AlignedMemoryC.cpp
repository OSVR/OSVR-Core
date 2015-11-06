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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Util/AlignedMemoryC.h>
#include <osvr/Util/StdAlignWrapper.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdlib.h>
#include <cassert>

void *osvrAlignedAlloc(size_t bytes, size_t alignment) {
    // Allocate a memory buffer with enough space to store a pointer to the
    // original buffer.
    auto space = bytes + alignment;
    void *buffer = malloc(space + sizeof(void *));
    if (!buffer) {
        return nullptr;
    }

    // Reserve the first byte for our buffer pointer for later freeing.
    void *alignBase = (void **)buffer + 1;

    // After this call the 'alignBase' pointer will be aligned to a boundary.
    // If there is not enough space to align the pointer, it stays
    // unaligned, and nullptr is returned insted of the updated alignBase.
    void *ret = osvr::align(alignment, bytes, alignBase, space);

    if (space < bytes || !ret) {
        free(buffer);
        return nullptr;
    }

    // Store the buffer pointer for the free call.
    ((void **)ret)[-1] = buffer;

    return ret;
}

void osvrAlignedFree(void *p) {
    // Null-pointer should be a no-op.
    if (p) {
        free(((void **)p)[-1]);
    }
}
